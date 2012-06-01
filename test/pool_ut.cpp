#include "HandyUtils.hpp"
#include "handy_pool.hpp"
#include "handy_pool_types.hpp"
#include "handy_ref.hpp"
#include "handy_string.hpp"
#include "handy_functor.hpp"
#include <deque>
#include <list>

using namespace std;
using namespace handy_utils;

#include <malloc.h>

void output_malloc_info()
{
    struct mallinfo m;
    m=mallinfo();
    LOG(1, "=================");
    LOG(1, "arena:"<<m.arena );
    LOG(1, "ordblks:"<<m.ordblks);
//    cout << "smblks:"<<m.smblks<<endl;
    LOG(1, "hblks:"<<m.hblks);
    LOG(1,"hblkhd:"<<m.hblkhd);
//    cout << "usmblks:"<<m.usmblks<<endl;
//    cout << "fsmblks:"<<m.fsmblks<<endl;
    LOG(1,"uordblks:"<<m.uordblks);
    LOG(1, "fordblks:"<<m.fordblks);
    LOG(1,"keepcost:"<<m.keepcost);
}

size_t get_mem()
{
//    return 0;
#if 1
    //output_malloc_info();

    unsigned long rss=0;
    string pid=to_<string>(getpid());
    string memfn=string("memchk.")+pid;
    system((string("ps -o rss -p ")+pid+" > "+memfn).c_str());
    fstream f(memfn.c_str(), ios_base::in);
    string line;

    line=strip(readline<string>(f));
    if(line!="RSS"){
        LOG(1, "bad rss:"<<line);
    }

    line=strip(readline<string>(f));
    f.close();
    if(get_unsigned(rss, line)){
        return (size_t)rss;
    }
    else{
        ERR("bad rss value:"<<line);
    }
#endif
}

int list_header_ut()
{
    cout << "-> list_header_ut" << endl;
    list_header a(1),b,c;
    b.insert_after(&a);
    c.insert_before(&a);
    THROW_IF(a.next()!=&b, "bad");
    THROW_IF(a.prev()!=&c, "bad");
    THROW_IF(c.prev()!=&b, "bad");
    THROW_IF(c.next()!=&a, "bad");
    THROW_IF(b.prev()!=&a, "bad");
    THROW_IF(b.next()!=&c, "bad");
    b.erase_from_list();
    THROW_IF(a.next()!=&c, "bad");
    THROW_IF(a.prev()!=&c, "bad");
    THROW_IF(c.prev()!=&a, "bad");
    THROW_IF(c.next()!=&a, "bad");
    THROW_IF(b.prev()!=NULL, "bad");
    THROW_IF(b.next()!=NULL, "bad");

    return 0;
}

int pool_ut()
{
    size_t free_cnt, free_cap, empty_cap, full_cnt, k, m, n;

    cout << "-> pool_ut" << endl;
    mem_pool* g0=get_pool_<tmp_pool>();
    g0->print_info();
    void* r1=g0->malloc(1024*1024);
    g0->print_info();
    cout <<"step1:"<<get_mem()<<endl;
    pool_free(r1);
    g0->print_info();

    if(sizeof(long)==8){
        THROW_IF(g0->get_seg_cnt()!=63, "err");
        THROW_IF(g0->get_seg(0)->chunk_size()!=8, "err");
        THROW_IF(g0->get_max_chunk_size()!=138728, "err");
    }
    THROW_IF(g0->get_seg(g0->get_max_chunk_size()+1)->chunk_size()!=0, "err");

    void* p=g0->malloc(103,3);
    g0->print_info();

    seg_pool* ma=g0->get_seg(103*3);
    ma->get_info(free_cnt, free_cap, empty_cap, full_cnt);
    n=free_cap;
    THROW_IF(free_cnt!=1, "err");
    THROW_IF(empty_cap!=0, "err");
    THROW_IF(full_cnt!=0, "err");

    pool_free(p);
    g0->print_info();
    ma->get_info(free_cnt, free_cap, empty_cap, full_cnt);
    THROW_IF(free_cnt!=0, "err");
    THROW_IF(empty_cap!=n, "err");
    THROW_IF(full_cnt!=0, "err");

    g0->purge();
    g0->print_info();
    ma->get_info(free_cnt, free_cap, empty_cap, full_cnt);
    THROW_IF(free_cnt!=0, "err");
    THROW_IF(empty_cap!=0, "err");
    THROW_IF(full_cnt!=0, "err");

    void* q[10240];
    q[0]=g0->malloc(1,1);
    ma=g0->get_seg(1);
    ma->get_info(free_cnt, free_cap, empty_cap, full_cnt);
    THROW_IF(free_cnt!=1, "err");
    n=free_cap;
    THROW_IF(empty_cap!=0, "err");
    THROW_IF(full_cnt!=0, "err");

    if(n*2+1<10240){
        for(int i=1; i<n*2+1; i++)
            q[i]=g0->malloc(0,1);
        g0->print_info();
        ma->get_info(free_cnt, free_cap, empty_cap, full_cnt);
        THROW_IF(free_cnt!=1, "err");
        m=free_cap;
        THROW_IF(free_cap < n*2, "err");
        THROW_IF(empty_cap!=0, "err");
        THROW_IF(full_cnt!=n*2, "err");

        for(int i=10; i<n*2+1; i++)
            pool_free(q[i]);
        for(int i=10; i<100; i++)
            q[i]=g0->malloc(4,1);
        g0->print_info();
        ma->get_info(free_cnt, free_cap, empty_cap, full_cnt);
        THROW_IF(free_cnt!=100, "err");
        THROW_IF(free_cap != n, "err");
        THROW_IF(empty_cap!= m, "err");
        THROW_IF(full_cnt!=0, "err");

        void* r=g0->malloc(128*1024);
        for(int i=0; i<100; i++)
            pool_free(q[i]);
        g0->print_info();
        ma->get_info(free_cnt, free_cap, empty_cap, full_cnt);
        THROW_IF(free_cnt!=0, "err");
        THROW_IF(free_cap != 0, "err");
        THROW_IF(empty_cap!= m, "err");
        THROW_IF(full_cnt!=0, "err");

        ma=g0->get_seg(128*1024);
        ma->get_info(free_cnt, free_cap, empty_cap, full_cnt);
        THROW_IF(free_cnt!=0, "err");
        THROW_IF(free_cap != 0, "err");
        THROW_IF(empty_cap!= 0, "err");
        THROW_IF(full_cnt!=1, "err");

        pool_free(r);
    }
    else{
        LOG(0, "too big to test:"<<n);
    }
    cout <<"step1:"<<get_mem()<<endl;
    g0->destroy();
    return 0;
}

void string_ut1(mem_pool* g0)
{
    tstring n="";
    tstring m="a";
    tstring a="abc";
    tstring a1="aadfasdfasdfasfdasdfadsfasdfasdfasdfasdfadsfasdfasdfasdfasdfbc";
    cout << m<<":"<< n<<":"<< a << ":" << a1 << endl;
    g0->print_info();
}

int string_ut()
{
    cout << "-> string_ut" << endl;
    mem_pool* g0=get_pool_<tmp_pool>();
    g0->print_info();
    string_ut1(g0);
    g0->print_info();
        return 0;
}

int vector_ut()
{
    cout << "-> vector_ut" << endl;
    mem_pool* g0=get_pool_<tmp_pool>();
    //vector<tstring, meta_allocator<tstring, tmp_pool> > v;
    tvector(tstring) v;
    cout << v.capacity() << endl;
    v.push_back("abc");
    g0->purge();
    g0->print_info();
    for(int i=1; i< 100; i++)
        v.push_back("abc");
    g0->purge();
    g0->print_info();
    v.clear();
    g0->purge();
    g0->print_info();
    v.resize(0);
    g0->purge();
    g0->print_info();
    cout << v.capacity() << endl;
    return 0;
}

int deque_ut()
{
    cout << "-> deque_ut" << endl;
    mem_pool* g0=get_pool_<tmp_pool>();
    tdeque(tstring) v;
    v.push_back("abc");
    g0->purge();
    g0->print_info();
    for(int i=1; i< 100; i++)
        v.push_back("abc");
    g0->purge();
    g0->print_info();
    v.clear();
    g0->purge();
    g0->print_info();
    v.resize(0);
    g0->purge();
    g0->print_info();
    return 0;
}

int list_ut()
{
    cout << "-> list_ut" << endl;
    mem_pool* g0=get_pool_<tmp_pool>();
    tlist(tstring) v;
    v.push_back("abc");
    g0->purge();
    g0->print_info();
    for(int i=1; i< 100; i++)
        v.push_back("abc");
    g0->purge();
    g0->print_info();
    v.clear();
    g0->purge();
    g0->print_info();
    return 0;
}

int map_ut()
{
    cout << "-> map_ut" << endl;
    mem_pool* g0=get_pool_<tmp_pool>();
    tmap(int,tstring) v;
    v[0]="abc";
    g0->purge();
    g0->print_info();
    for(int i=1; i< 100; i++)
        v[i]="abc";
    g0->purge();
    g0->print_info();
    v.clear();
    g0->purge();
    g0->print_info();
    return 0;
}

int comp_ut()
{
    cout << "-> comp_ut" << endl;
    mem_pool* g0=get_pool_<tmp_pool>();
    tmap(int,tvector(tstring)) v;
    v[0].push_back("abc");
    g0->purge();
    g0->print_info();
    for(int i=1; i< 100; i++)
        v[i].push_back("abc");
    g0->purge();
    g0->print_info();
    v.clear();
    g0->purge();
    g0->print_info();
    return 0;
}

int new_ut()
{
    cout << "-> new_ut" << endl;
    mem_pool* g0=get_pool_<tmp_pool>();
    tstring* p=tmp_new(tstring,("abc"));
    tstring* q=tmp_new(tstring,());
    g0->print_info();
    pool_delete(p);
    pool_delete(q);
    g0->purge();
    g0->print_info();

    return 0;
}


int string_op_ut()
{
    cout << "-> string_op_ut" << endl;

    tstring a=" adsf 123 456 ";
    THROW_IF(strip(a)!="adsf 123 456", "err");

    tlist(tstring) l;
    split(l, a);
    tstring r=join(" ", l);
    THROW_IF(r!="adsf 123 456", "err");
    THROW_IF(!startswith(r,"adsf"), "err");
    THROW_IF(startswith(r,"123"), "err");
    THROW_IF(!startswith(r,tstring("adsf")), "err");
    THROW_IF(startswith(r,tstring("123")), "err");
    THROW_IF(!endswith(r,"456"), "err");
    THROW_IF(endswith(r,"123"), "err");
    THROW_IF(!endswith(r,tstring("456")), "err");
    THROW_IF(endswith(r,tstring("123")), "err");

    split(l, a, "a14");
    THROW_IF(l.size()!=4, "err");

    tstring n=to_<tstring>(16, 16);
    THROW_IF(n!="0x10", "err");

    long k;
    THROW_IF(!get_int(k, n, 16), "err");
    THROW_IF(k!=16, "err");

    unsigned long u;
    THROW_IF(!get_unsigned(u, n, 16), "err");
    THROW_IF(u!=16, "err");

    THROW_IF(to_lower(r)!="adsf 123 456", "err");
    THROW_IF(to_upper(r)!="ADSF 123 456", "err");
    std::string r1="ABC";
    THROW_IF(to_lower(r1)!="abc", "err");
    THROW_IF(to_upper(r1)!="ABC", "err");
    {
        string a="abcdef";
        THROW_IF(!istartswith(a, "ABc"), "err");
        THROW_IF(!iendswith(a, "dEf"), "err");
    }
    return 0;
}

int trav_dir_ut()
{
    cout << "-> trav_dir_ut" << endl;
    tlist(tstring) files;
    trav_dir(files, ".","*");
    tlist(tstring)::iterator it=files.begin();
    bool found=false;
    for(;it!=files.end();++it){
        if(endswith(*it,"handy_string.hpp")){
            found=true;
        }
    }
    THROW_IF(!found, "err");
    return 0;
}


struct obj_test:obj_vref{
    string a;
    int b;
};

class test:public vref_<obj_test>{
public:
    DEF_VCTOR(test)

    void init(string a, int b)
    {
        enter_new();
        DEF_SELF;
        self.a=a;
        self.b=b;
    }

    test(string a, int b)
    {
        init(a,b);
    }

};

ostream& operator<<(ostream& s, const test& t)
{
    s << t->a << ","<< t->b << std::endl;
    return s;
}

struct obj_derived:obj_test{
    string c;
};

class derived:public vref_<obj_derived, test>{
public:
    DEF_VCTOR(derived)
        
    void init(string a, int b, string c)
    {
        test::init(a,b);
        DEF_SELF;
        self.c=c;
    }

    derived(string a, int b, string c)
    {
        init(a,b,c);
    }
};

DEF_VCLASS(der, obj_derived);

struct obj_de{
    string a;
    int b;
    string c;
};
DEF_CLASS(de, obj_de);

ostream& operator<<(ostream& s, const derived& t)
{
    s << t->a << ","<< t->b << "," << t->c << std::endl;
    return s;
}

int ref_ut()
{
    std::cout << "-> ref_ut" << std::endl;

    test t("c",10);
    std::cout << t;

    derived d("a",1,"daf"), e(d.copy());
    std::cout << d << e;

    bool k1=false;
    try{
        e=t;
    }
    catch(err&){
        k1=true;
    }
    THROW_IF(!k1, "no cast err detected!");

    der f(dumb);
    f->a="abc"; f->b=2; f->c="def";
    std::cout << f->a << ", " << f->b << ", " << f->c << std::endl;

    de g(dumb);
    g->a="dkf"; g->b=3; g->c="dfe";
    de j(g), k(g.copy());
    std::cout << g->a << ", " << g->b << ", " << g->c << std::endl;
    std::cout << j->a << ", " << j->b << ", " << j->c << std::endl;
    std::cout << k->a << ", " << k->b << ", " << k->c << std::endl;

    return 0;
}

volatile int refc_count=0;

struct obj_refc_test{
    int a;
    obj_refc_test():a(0)
    {
        refc_count++;
    }

    ~obj_refc_test()
    {
        refc_count--;
    }
};

class ref_test:public ref_<obj_refc_test>{
public:
    DEF_CTOR(ref_test);
    ref_test(int a)
    {
        enter_new();
        DEF_SELF;

        self.a=a;
    }
};

int ref_test_ut()
{
    std::cout << "-> ref_test_ut" << std::endl;
    {
        tvector(ref_test) as;
        for(int i=0; i<10;i++){
            ref_test a(2);
            as.push_back(a);
        }
        //std::cout << refc_count << std::endl;
        THROW_IF(refc_count!=10, "bad refc_count");
        ref_test b;
        b=as[0];
        as.clear();
        //std::cout << refc_count << std::endl;
        THROW_IF(refc_count!=1, "bad refc_count");
        b.release();
        THROW_IF(refc_count!=0, "bad refc_count");
    }
    //std::cout << "refc_count:"<<refc_count << std::endl;
    return 0;
}

int string_cast_ut()
{
    std::cout << "-> string_cast_ut" << std::endl;

    tstring a="abc";
    estring b=to_<estring>(a);

    std::cout <<  b << endl;
    THROW_IF(strcmp(a.c_str(), b.c_str())!=0, "err");

    return 0;
}

int pool_idx_ut()
{
    std::cout << "-> pool_idx_ut" << std::endl;
    mem_pool* g0=get_pool_<tmp_pool>();
    seg_pool* seg;
    seg=g0->get_seg(0);
    THROW_IF(seg->chunk_min_size()>0, "err");
    size_t i=0; 
    while(1){
        seg=g0->get_seg(i);
        if(seg->chunk_size()==0){
            std::cout << "max:"<<i<<std::endl;
            break;
        }
        THROW_IF(seg->chunk_min_size()>i || i> seg->chunk_size(), "err:"<<i<<"("<<seg->chunk_min_size()<<","<<seg->chunk_size()<<")");
        i++;
    }
    return 0;
}


class fc {
public:
    int k(int t)
    {
        return t;
    }
    DEF_FUNCTOR(fc, int, k, (int) );

    void n()
    {
        std::cout << "n" << std::endl;
    }
    DEF_FUNCTOR(fc, void, n, () );

    int m(int a, int b)
    {
        return a+b;
    }
    DEF_FUNCTOR(fc, int, m, (int a, int b) );
};

int k1(int t)
{
    return t-1;
}

void n1(int t)
{
    std::cout << t-1 << std::endl;
}

void push_seq(int x1, int x2, int x3)
{
    std::cout << "push seq:" << &x1 << "," << &x2 << "," << &x3 << std::endl;
}

int functor_ut()
{
    std::cout << "-> functor_ut" << std::endl;

    fc f;
    //push_seq(1,2,3);

    functor_<int(int)> k=fc::k_fp(f);
    functor_<void()> t=fc::n_fp(f);
    std::cout << k(3) << std::endl;
    THROW_IF(k(3)!=3, "err");
    t();

    functor_<int(int,int)> s=fc::m_fp(f);
    THROW_IF(s(3,4)!=7, "err");

    k=k1;
    functor_<int(int)> bf=k1;
    std::cout << typeid(bf).name() << std::endl;
    std::cout << bf(3) << std::endl;
    std::cout << k(3) << std::endl;

    bf=k;
    std::cout << bf(3) << std::endl;

    //std::cout << (bf==k )<< std::endl;
    THROW_IF(!(bf==k), "err");
    THROW_IF(bf!=k, "err");

    THROW_IF(sizeof(t)!=sizeof(s),"err");
    return 0;
}

int ut()
{

    unittest_t a[]={list_header_ut,
                    pool_ut,
                    string_ut,
                    vector_ut,
                    deque_ut,
                    list_ut,
                    map_ut,
                    comp_ut,
                    new_ut,
                    string_op_ut,
                    trav_dir_ut,
                    ref_ut,
                    ref_test_ut,
                    string_cast_ut,
                    pool_idx_ut,
                    functor_ut,
                    NULL};

    int r= unittest_run(a);
    return r;
}

int main()
{
    debug_level=1;
    mem_pool* g=get_pool_<tmp_pool>();
    cout <<"begin:"<<get_mem()<<endl;
    ut();
    cout << "sizeof(mem_pool):"<<sizeof(mem_pool)<<endl;
    cout << "sizeof(size_t):"<<sizeof(size_t)<<endl;
    cout <<"end:"<<get_mem()<<endl;
}
