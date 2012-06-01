#ifndef PROTON_POOL_HEADER
#define PROTON_POOL_HEADER
#include <new>

namespace proton{

struct tag_pool_temp{};

struct tag_pool_pers{};

class pool_block;
class seg_pool;
class mem_pool;

union chunk_header{
    chunk_header* next_free;
    pool_block* parent; ///< NULL means being malloc-ed directly
};

class list_header {
protected:
    list_header* _prev;
    list_header* _next;

private:
    list_header(const list_header& lh); ///< disabled
public:
    list_header():_prev(NULL), _next(NULL)
    {}

    /// init as a circle
    list_header(int):_prev(this), _next(this)
    {}

    bool empty()const
    {
        return (_next==this || _next==NULL);
    }

    void erase_from_list()
    {
        if(_prev)
            _prev->_next=_next;
        if(_next)
            _next->_prev=_prev;
        _prev=NULL;
        _next=NULL;
    }

    ~list_header()
    {
        erase_from_list();
    }

    list_header* next()
    {
        return _next;
    }

    list_header* prev()
    {
        return _prev;
    }

    void insert_after(list_header* prev)
    {
        THROW_IF(_prev, "invalid prev");
        THROW_IF(_next, "invalid next");    

        _prev=prev;
        if(prev){
            _next=prev->_next;
            prev->_next=this;
        }
        if(_next)
            _next->_prev=this;
    }

    void insert_before(list_header* next)
    {
        THROW_IF(_prev, "invalid prev");
        THROW_IF(_next, "invalid next");    

        _next=next;
        if(_next){
            _prev=next->_prev;
            _next->_prev=this;
        }
        if(_prev){
            _prev->_next=this;
        }
    }
};


inline pool_block* get_block(list_header* lh)
{
    return (pool_block*)(lh);
}

class pool_block:public list_header {
    friend class seg_pool;
protected:

    seg_pool* _parent;
    size_t _block_size; // 4
    size_t _chunk_size; 
    size_t _chunk_cap;

    size_t _chunk_cnt;
    size_t _chunk_max;  // 4

    char* _unalloc_chunk;
    chunk_header* _free_header;
    // chunk_header   // 3 + 1 of mmheader
    // data
private:
    pool_block(const pool_block& a); ///< disabled
public:
    pool_block(size_t chunk_size, size_t block_size, seg_pool* parent);
    ~pool_block();

    void* malloc_one();
    void free_chunk(chunk_header *ch);

    seg_pool* parent()
    {
        return _parent;
    }
    size_t block_size()
    {
        return _block_size;
    }
    pool_block* prev_block()
    {
        return get_block(_prev);
    }
    pool_block* next_block()
    {
        return get_block(_next);
    }

    bool full()
    {
        return _chunk_cnt==_chunk_cap;
    }
    bool empty()
    {
        return _chunk_cnt==0;
    }

};


class seg_pool {
    friend class pool_block;
    friend class mem_pool;
    friend void pool_free(void* p);
protected:
    mem_pool* _parent;

    size_t _chunk_size; // 0 means: new directly
    size_t _chunk_min_size; // _chunk_min_size <= real_size <=_chunk_size

    list_header _free_blocks;
    list_header _full_blocks;
    list_header _empty_blocks;

    size_t _total_block_size;
    size_t _min_block_size;

    pool_block* get_free_block()
    {
        list_header* p=_free_blocks.next();
        if(p==&_free_blocks)
            return NULL;
        else
            return get_block(p);
    }

    void malloc_block();
    void release_block(pool_block* p);

    void reg_free_block(pool_block* p);
    void reg_full_block(pool_block* p);
    void reg_empty_block(pool_block* p);

    void free_chunk(chunk_header* ch);
    void purge_circle(list_header* lh);

private:
    seg_pool(const seg_pool& a); ///< disabled
public:
    seg_pool();
    ~seg_pool();

    void init(size_t chunk_size, size_t chunk_min_size, mem_pool* parent);
    void destroy();
    void purge();

    size_t chunk_size()const
    {
        return _chunk_size;
    }
    size_t chunk_min_size()const
    {
        return _chunk_min_size;
    }

    void* malloc(size_t size, size_t n=1);

    void get_info(size_t&free_cnt, size_t& free_cap, size_t& empty_cap, size_t& full_cnt);
    void print_info(bool print_null);
};

#define handy_meta_block_max 128

class mem_pool {
protected:
    seg_pool _segs[handy_meta_block_max+1]; 
    size_t _seg_cnt;
    size_t _seg_linear_cnt;

    void compute_sizes(size_t max, size_t align, size_t factor);

private:
    mem_pool(const mem_pool& a); ///< disabled

public:
    mem_pool(size_t max=16*1024*sizeof(long), size_t factor=8);
    ~mem_pool();

    void destroy();
    void purge();

    void* malloc(size_t size, size_t n=1); // malloc size*n

    seg_pool* get_seg(size_t size);

    size_t get_seg_cnt()const
    {
        return _seg_cnt;
    }
    size_t get_max_chunk_size()const
    {
        return _segs[_seg_cnt-1].chunk_size();
    }
    
    size_t get_seg_total(); ///< get total memory usage of seg pools
    size_t get_seg_free();  ///< get total free allocatable memory of seg pools

    void print_info();
};

void pool_free(void* p);

/////////////////////////////////////////////////////
// pools

template<typename pool_tag>mem_pool* get_pool_()
{
    static mem_pool alloc;
    return &alloc;
}

struct tmp_pool {}; // temporary pool
struct per_pool {}; // persistent pool

inline void* tmp_malloc(size_t size)
{
    return get_pool_<tmp_pool>()->malloc(size);
}

inline void* per_malloc(size_t size)
{
    return get_pool_<per_pool>()->malloc(size);
}

#define tmp_new(T,arg) (new(tmp_malloc(sizeof(T))) T arg)
#define per_new(T,arg) (new(per_malloc(sizeof(T))) T arg)
template<typename T> void pool_delete(T* p)
{
    p->~T();
    pool_free((void*)p);
}

//////////////////////////////////////////////////////////
//  STL allocator

template<class T, typename pool_tag=tmp_pool> class meta_allocator {
public:
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef T           value_type;
    template<class U>struct rebind{
        typedef meta_allocator<U, pool_tag> other;
    };

    meta_allocator()
    {
    }

    // default copy constructor
    // default assignment operator
    // default destructor

    template<class U> meta_allocator(const meta_allocator<U, pool_tag>&)
    {
    }

    static pointer address(reference x)
    {
        return &x;
    }
    static const_pointer address(const_reference x)
    {
        return &x;
    }

    static size_type max_size()
    {
        return size_type(-1);
    }

    static pointer allocate(size_type n)
    {
        static seg_pool* meta=get_pool_<pool_tag>()->get_seg(sizeof(value_type));
        pointer r=(pointer)meta->malloc(sizeof(T), n); 
        if(!r)
            throw std::bad_alloc();
        return r;
    }

    static pointer allocate(size_type n, const void * const)
    {
        return allocate(n);
    }

    static void deallocate(pointer p, size_type n)
    {
        if(p)
            pool_free(p);
    }

    static void construct(pointer p, const T& val)
    {
        new (p) T(val);
    }

    static void destroy(pointer p)
    {
        p->~T();
    }

    bool operator==(const meta_allocator &) const
    { return true; }

    bool operator!=(const meta_allocator &) const
    { return false; }

};

template<class _Ty, class _Other, typename pool_type> inline
	bool operator==(const meta_allocator<_Ty,pool_type>&, const meta_allocator<_Other,pool_type>&)
	{	// test for allocator equality (always true)
	return (true);
	}

template<class _Ty, class _Other, typename pool_type> inline
	bool operator!=(const meta_allocator<_Ty,pool_type>&, const meta_allocator<_Other,pool_type>&)
	{	// test for allocator inequality (always false)
	return (false);
	}

template<typename pool_tag>class meta_allocator<void, pool_tag>
{
public:
    typedef void*       pointer;
    typedef const void* const_pointer;
    typedef void        value_type;
    template <class U> struct rebind {
        typedef meta_allocator<U, pool_tag> other;
    };
};

};
#endif // PROTON_POOL_HEADER
