#include <cstdlib>
#include <proton/base.hpp>
#include <proton/pool.hpp>

#ifdef __linux__
#include <sys/mman.h>
#else
#define mmap(a, s, b, c, d, e) malloc(s)
#define MAP_FAILED NULL
#endif

using namespace proton::detail;

namespace proton{

constexpr size_t block_size_initial=8*1024; // 8K
constexpr size_t block_size_max=512*1024; // 512K
constexpr size_t page_align=4096;
constexpr size_t chunk_align=(2*sizeof(long));

namespace detail{
/////////////////////////////////////////////////
/// utils

struct mmheader{
    size_t len;
};

void* mmalloc(size_t s)
{
    s+=sizeof(mmheader);
    mmheader* r=(mmheader*)mmap(NULL, s, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS,
        -1, 0);
    if(r!=MAP_FAILED){
        r->len=s;
        return (void*)(r+1);
    }
    else{
        PROTON_LOG(0, "mmalloc failed");
        return NULL;
    }
}

void* __mmdup(void* p)
{
    mmheader* q=((mmheader*)p)-1;
    size_t s=q->len;
    return mmalloc(s-sizeof(mmheader));
}

void mmfree(void* p)
{
    mmheader* r=((mmheader*)p)-1;
#ifdef __linux__
    int ret=munmap((void*)r, r->len);
    if(ret){
        PROTON_LOG(0, "munmap failed:"<<ret);
    }
#else
    free(r);
#endif
}

size_t get_heap_header_size()
{
#ifdef __linux__
    return sizeof(mmheader);
#else
    static int size=-1;
    if(size<0){
        void* p = malloc(block_size_initial);
        if(p!=NULL){
            size= (size_t)p - (((size_t)p)&((size_t)(-1)-page_align+1));
            free(p);
        }
        else{
            PROTON_LOG(0, "err: can't malloc() during get_heap_header_size()");
            size=32;
        }
    }
    return size;
#endif
}
} // ns detail

/////////////////////////////////////////////////
/// mem_pool

mem_pool::mem_pool(size_t max/*=32*1024*/, size_t factor/*=16*/)
    :_seg_cnt(0), _seg_linear_cnt(0)
{
    compute_sizes(max, chunk_align, factor);
}

mem_pool::~mem_pool()
{
    destroy();
}

inline size_t block_size(size_t typesize, size_t align, size_t factor)
{
    typesize=typesize+sizeof(chunk_header);
    size_t ceil=typesize+typesize/factor;
    size_t size1=(ceil+align-1) & ((size_t)(-1)-align+1);
    return size1-sizeof(chunk_header);
}

void mem_pool::compute_sizes(size_t max, size_t align, size_t factor)
{
    size_t s=0;
    size_t i=0;
    while(i<max){
        s=block_size(i, align, factor);
        _segs[_seg_cnt].init(s,i, this);
        _seg_cnt++;
        if(s-i <= align)
            _seg_linear_cnt++;
        if(_seg_cnt>=PROTON_META_BLOCK_MAX){
            PROTON_LOG(0, "mem_pool::compute_sizes PROTON_META_BLOCK_MAX is not enough");
            break;
        }
        i=s+1;
    }
    _segs[_seg_cnt].init(0,0,this);
}

void mem_pool::destroy()
{
    seg_pool* p=&_segs[0];
    for(size_t i=0; i<=_seg_cnt; i++, p++){
        p->destroy();
    }
}

void mem_pool::purge()
{
    seg_pool* p=&_segs[0];
    for(size_t i=0; i<=_seg_cnt; i++, p++){
        p->purge();
    }
}

size_t mem_pool::get_seg_total()
{
    size_t s=0;
    seg_pool* p=&_segs[0];
    for(size_t i=0; i<=_seg_cnt; i++, p++){
        s=s+p->_total_block_size;
    }
    return s;
}

size_t mem_pool::get_seg_free()
{
    size_t s=0;
    seg_pool* p=&_segs[0];
    for(size_t i=0; i<=_seg_cnt; i++, p++){
        size_t free_cnt, free_cap, empty_cap, full_cnt;
        p->get_info(free_cnt, free_cap, empty_cap, full_cnt);
        s=s+(free_cap-free_cnt+empty_cap)*p->chunk_size();
    }
    return s;
}

seg_pool* mem_pool::get_seg(size_t size)
{
    if( size<=_seg_linear_cnt*chunk_align-sizeof(chunk_header)){
        size_t idx= (size +sizeof(chunk_header)-1) / chunk_align;
        return &_segs[idx];
    }
    else if(size>_segs[_seg_cnt-1].chunk_size()){
        return &_segs[_seg_cnt];
    }
    else{
        size_t begin=_seg_linear_cnt-1; // >
        size_t end=_seg_cnt-1; // <=
        while(begin+1!=end){
            size_t mid=(end+begin)/2;
            if(size>_segs[mid].chunk_size())
                begin=mid;
            else
                end=mid;
        }
        return &_segs[end];
    }
}

void* mem_pool::malloc(size_t size, size_t n/*=1*/)
{
    size_t real_size;
    if(n==1)
        real_size=size;
    else{
        real_size=size*n;

        if(n!=0 && real_size/n!=size){
            PROTON_LOG(0, "size overflow:"<<size<<"*"<<n);
            return NULL;
        }
    }
    seg_pool* meta=get_seg(real_size);
    // [TODO] optimize to use malloc_one
    return meta->malloc(real_size);
}

void mem_pool::print_info()
{
    static bool print_null=true;
    std::cout << "=mem pool== " << get_seg_total() << " ( "
        << get_seg_free() <<" )==========================\n";
    if(print_null)
        std::cout << "_seg_cnt: " << _seg_cnt << "\n";
    for(size_t i=0; i<=_seg_cnt; i++){
        _segs[i].print_info(print_null);
    }
    print_null=false;
}

namespace detail{

/////////////////////////////////////////////////
/// seg_pool

seg_pool::seg_pool()
    :_parent(NULL), _chunk_size(0), _free_blocks(1), _empty_blocks(1),
        _full_blocks(1), _total_block_size(0)
{}

seg_pool::~seg_pool()
{
    destroy();
    //_parent=NULL;
    _chunk_size=0;
    _total_block_size=0;
}

void seg_pool::init(size_t chunk_size, size_t chunk_min_size, mem_pool* parent)
{
    _chunk_size=chunk_size;
    _chunk_min_size=chunk_min_size;
    _parent=parent;
    _min_block_size=chunk_size+get_heap_header_size()+sizeof(pool_block)
        +sizeof(chunk_header);
    if(block_size_initial > _min_block_size)
        _min_block_size=block_size_initial;
}

void* seg_pool::malloc_one()
{
    pool_block* ba=get_free_block();
    if(!ba){
        malloc_block();
        ba=get_free_block();
        if(!ba)
            return NULL;
    }
    void* p=ba->malloc_one();

    if(ba->full()){
        ba->erase_from_list();
        reg_full_block(ba);
    }
    return p;
}

void* seg_pool::malloc(size_t size, size_t n)
{
    if(_chunk_size>0){
        if(n==1 && size >= _chunk_min_size && size <= _chunk_size){
            return malloc_one();
        }
        else if(n>1){
            size_t real_size=size*n;
            if(real_size/n!=size){
                PROTON_LOG(0, "overflow:"<<size<<"*"<<n);
                return NULL;
            }
            if(real_size >= _chunk_min_size && real_size<=_chunk_size)
                return malloc_one();
            else
                return _parent->malloc(real_size);
        }
        else//(n==0)
            return _parent->malloc(0);
    }
    else{// _chunk_size==0, malloc directly
        size_t real_size;
        if(n==1){
            real_size=size;
        }
        else if(n>1) {
            real_size=size*n;
            if( real_size/n!=size){
                PROTON_LOG(0, "overflow:"<<size<<"*"<<n);
                return NULL;
            }
        }
        else{
            return _parent->malloc(0);
        }
        chunk_header* p=(chunk_header*)mmalloc(real_size+sizeof(chunk_header));
        p->parent=NULL;
        return (void*)(p+1);
    }
}

void seg_pool::free_chunk(chunk_header* ch)
{
    pool_block* ba=ch->parent;
    // ASSERT ba->parent()==this
    bool f=ba->full();
    ba->free_chunk(ch);
    if(ba->empty()){
        ba->erase_from_list();
        reg_empty_block(ba);
    }
    else if(f){
        ba->erase_from_list();
        reg_free_block(ba);
    }
}

void seg_pool::malloc_block()
{
    if(!_empty_blocks.empty()){
       pool_block* ba=get_block(_empty_blocks.next());
       ba->erase_from_list();
       reg_free_block(ba);
    }
    else{
        size_t new_size=_total_block_size;
        if(new_size < _min_block_size)
            new_size = _min_block_size;
        else if(new_size > block_size_max)
            new_size = block_size_max;
        size_t new_size1 = new_size-get_heap_header_size();
        void* newblock=mmalloc(new_size1);
        pool_block* ba=new (newblock) pool_block(_chunk_size, new_size1, this);
        if(ba){
            reg_free_block(ba);
            _total_block_size+=new_size;
        }
    }
}

void seg_pool::reg_free_block(pool_block* p)
{
    PROTON_POOL_THROW_IF(p->full(), "try to reg a full block:"<<p);
    p->insert_before(&_free_blocks);
}

void seg_pool::reg_full_block(pool_block* p)
{
    PROTON_POOL_THROW_IF(!p->full(), "try to reg an empty block into full_blocks:"<<p);
    p->insert_after(&_full_blocks);
}

void seg_pool::reg_empty_block(pool_block* p)
{
    // keep only one largest empty block.
    // [TODO] use simpler structure to record empty block.
    PROTON_POOL_THROW_IF(!p->empty(), "try to reg a not empty block into empty_blocks:"<<p);
    if(_empty_blocks.empty()){
        p->insert_after(&_empty_blocks);
        return;
    }
    else{
        pool_block* ba=get_block(_empty_blocks.next());
        if(ba->block_size() < p->block_size()){
            release_block(ba);
            p->insert_after(&_empty_blocks);
        }
        else{
            release_block(p);
        }
    }
}

void seg_pool::release_block(pool_block* p)
{
    _total_block_size-=p->block_size()+get_heap_header_size();
    p->erase_from_list();
    p->~pool_block();
    mmfree((void*)p);
}

void seg_pool::purge_circle(list_header* lh)
{
    while(!lh->empty()){
        release_block(get_block(lh->next()));
    }
}

void seg_pool::purge()
{
    purge_circle(&_empty_blocks);
}

void seg_pool::destroy()
{
    purge_circle(&_empty_blocks);
    purge_circle(&_free_blocks);
    purge_circle(&_full_blocks);
}

void seg_pool::get_info(size_t&free_cnt, size_t& free_cap, size_t& empty_cap, size_t& full_cnt)
{
    {
        size_t chunk_cnt=0, chunk_total=0;
        list_header* lh=_free_blocks.next();
        while(lh!=&_free_blocks){
            pool_block* ba=get_block(lh);
            chunk_cnt+=ba->_chunk_cnt;
            chunk_total+=ba->_chunk_cap;
            lh=lh->next();
        }
        free_cnt=chunk_cnt;
        free_cap=chunk_total;
    }
    {
        size_t chunk_cnt=0, chunk_total=0;
        list_header* lh=_full_blocks.next();
        while(lh!=&_full_blocks){
            pool_block* ba=get_block(lh);
            chunk_cnt+=ba->_chunk_cnt;
            chunk_total+=ba->_chunk_cap;
            lh=lh->next();
        }
        PROTON_POOL_THROW_IF(chunk_cnt!=chunk_total, "bad full");
        full_cnt=chunk_cnt;
    }
    {
        size_t chunk_cnt=0, chunk_total=0;
        list_header* lh=_empty_blocks.next();
        while(lh!=&_empty_blocks){
            pool_block* ba=get_block(lh);
            chunk_cnt+=ba->_chunk_cnt;
            chunk_total+=ba->_chunk_cap;
            lh=lh->next();
        }
        PROTON_POOL_THROW_IF(chunk_cnt!=0, "bad empty");
        empty_cap=chunk_total;
    }
}

void seg_pool::print_info(bool print_null)
{
    if(_total_block_size || print_null)
        std::cout << "--seg: " << _chunk_min_size << " - " << _chunk_size <<
            " - ( " << _min_block_size << " | " << _total_block_size << " )\n";
    if(_total_block_size){
        size_t free_cnt, free_cap, empty_cap, full_cnt;
        get_info(free_cnt, free_cap, empty_cap, full_cnt);
        std::cout << "free: " << free_cnt << "/" << free_cap << ", empty: "
            << empty_cap << ", full: " << full_cnt << "\n";
    }
}

/////////////////////////////////////////////////
/// pool_block

pool_block::pool_block(size_t chunk_size, size_t block_size, seg_pool* parent)
    : _parent(parent), _block_size(block_size), _chunk_size(chunk_size),
     _chunk_cnt(0), _chunk_max(0), _free_header(NULL)
{
    _chunk_cap=(block_size-sizeof(pool_block))/(_chunk_size+sizeof(chunk_header));
    _unalloc_chunk=(char*)(this+1);
}

pool_block::~pool_block()
{
}

void* pool_block::malloc_one()
{
    if(_free_header){
        chunk_header* p=_free_header;
        _free_header=_free_header->next_free;
        _chunk_cnt++;
        p->parent=this;
        return (void*)(p+1);
    }
    else if(_chunk_max<_chunk_cap){
        chunk_header* p=(chunk_header*)_unalloc_chunk;
        p->parent=this;

        _unalloc_chunk+=_chunk_size+sizeof(chunk_header);
        _chunk_max++;
        _chunk_cnt++;

        return (void*)(p+1);
    }
    else{
        PROTON_LOG(0, "bad alloc in a full block");
        return NULL;
    }
}

void pool_block::free_chunk(chunk_header *ch)
{
    PROTON_POOL_THROW_IF(ch->parent!=this, "unmatched:"<<ch->parent<<" vs. "<<this);

    ch->next_free=_free_header;
    _free_header=ch;
    _chunk_cnt--;
}

} // ns detail
} // ns proton
