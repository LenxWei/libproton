#ifndef PROTON_REF_HEADER
#define PROTON_REF_HEADER

#include <memory>
#include <utility>
#include <functional>
#include <tuple>
#include <type_traits>
#include <proton/pool.hpp>

namespace proton{

//////////////////////////////
// lite ref obj
// TODO: o() valid check, object counting

class refc_t {
private:
    long __r;
public:
    refc_t():__r(0)
    {}

    refc_t(const refc_t& r):__r(0)
    {}

    refc_t& operator=(const refc_t& r)
    {
        return *this;
    }

    bool operator<(long i)const
    {
        return __r < i;
    }

    bool operator==(long i)const
    {
        return __r == i;
    }

    long enter()
    {
        return ++__r;
    }

    long release()
    {
        return --__r;
    }

    long count() const
    {
        return __r;
    }
};

template<typename objT> class __refc_{
public:
    refc_t __r;
    objT o;

    template<typename ...argT> __refc_(argT&& ...a):o(a...)
    {}
};

class init_alloc{};
extern init_alloc alloc;

template<typename refT> bool is_null(const refT& x)
{
    return x.__rp()==NULL;
}

template<typename refT> bool is_valid(const refT& x)
{
    return x.__rp()!=NULL;
}

/** Generate a copy of object.
 * [FIXME] need to be rewrited to support derived types
 */
template<typename refT> refT copy(const refT& x)
{
    if(is_null(x))
        return refT;
    typename refT::refc_obj_t* p;
    refT::allocate(p);
    new (p) (typename refT::refc_obj_t)(x.__o());
    return refT(alloc,p);
}

template<typename refT> void reset(refT& x)
{
    x.__release();
}

template<typename refT> int ref_count(const refT& x)
{
    if(x.__rp())
        return x.__rp()->__r.count();
    else
        return 0;
}

/** The core reference support template.
 * @param allocator must support confiscate(), see smart_allocator in <proton/pool.hpp>
 */
template<typename objT, typename allocator=smart_allocator<objT> > struct ref_ {
public:
    typedef ref_ proton_ref_self_t;
    typedef std::ostream proton_ostream_t;
    typedef __refc_<objT> refc_obj_t;
    typedef objT obj_t;

    /// when is_new is false, deallocate the pointer
    inline static void allocate(refc_obj_t* & ptr)
    {
        static typename allocator::template rebind<refc_obj_t>::other a;
        ptr=a.allocate(1);
    }

    inline static void deallocate(refc_t* ptr)
    {
        allocator::confiscate((void*)ptr);
    }

protected:
    refc_t * _rp;
    objT*    _p;

protected:
    void enter(refc_t* p)
    {
        _rp=p;
        if(_rp)
            _rp->__r.enter();
    }

    void assign(refc_obj_t* p)
    {
        if(p!=_rp){
            // [TODO] change to swap mechanism
            __release();
            enter(p);
        }
    }

public:
    void __release()
    {
        if(_rp){
            int r=_rp->__r.release();
            if(!r){
                _p->~objT();
                deallocate(_rp);
            }
            _rp=NULL;
        }
    }

public:
    // ctors
    ref_():_rp(NULL)
    {}

    ref_(init_alloc, refc_t* rp, objT* p):_p(p)
    {
        enter(rp);
    }

    template<typename ...argT> explicit ref_(argT&& ...a)
    {
        refc_obj_t* p;
        alloc(p);
        new (p) refc_obj_t(a...);
        if(p){
            enter(p);
        }
    }

    template<typename ...argT> explicit ref_(init_alloc, argT&& ...a)
    {
        refc_obj_t* p;
        alloc(p);
        new (p) refc_obj_t(a...);
        if(p){
            enter(p);
        }
    }

    ref_(const ref_& r)
    {
        enter(r.__rp());
    }

    ref_(ref_& r)
    {
        enter(r.__rp());
    }

    ref_(ref_&& r)
    {
        _rp=r._rp;
        r._rp=NULL;
    }

    ref_& operator=(const ref_& r)
    {
        assign(r.__rp());
        return *this;
    }

    ~ref_()
    {
        __release();
    }


public:
    objT& __o()
    {
        return _rp->o;
    }

    const objT& __o()const
    {
        return _rp->o;
    }

    objT& operator *()
    {
        return __o();
    }

    const objT& operator *()const
    {
        return __o();
    }

    objT* operator->()
    {
        return &__o();
    }

    const objT* operator->()const
    {
        return &__o();
    }

    objT* __p()
    {
        if(_rp)
            return &__o();
        else
            return NULL;
    }

    const objT* __p()const
    {
        if(_rp)
            return &__o();
        else
            return NULL;
    }

    refc_obj_t* __rp()const
    {
        return _rp;
    }

};

/// need obj_t to implenment "ostream& output(ostream&)const"
template<typename T>std::ostream& operator<<(typename T::proton_ostream_t& s,
                                   const T& y)
{
    if(is_null(y)){
        s << "<>" ;
        return s;
    }
    y->output(s);
    return s;
}

template<typename T>bool operator<(const T&x,
                                   const typename T::keyed_self_t&y)
{
    return x.key() < y.key();
}

template<typename T>bool operator==(const T&x,
                                    const typename T::keyed_self_t&y)
{
    return x.key() == y.key();
}

template<typename T>bool operator<(const T&x,
                                   const typename T::proton_ref_self_t&y)
{
    if(is_null(x))
        return !is_null(y);
    if(is_null(y))
        return false;
    return x.__o() < y.__o();
}

template<typename T>bool operator==(const T&x,
                                    const typename T::proton_ref_self_t&y)
{
    if(is_null(x)){
        return is_null(y);
    }
    else{
        if(is_null(y))
            return false;
        else
            return x.__o() == y.__o();
    }
}

template<typename T>struct key_hash{
public:
    size_t operator()(const T& x)const
    {
        if(is_null(x))
            return 0;
        else{
            typedef decltype(x->key()) ref_t;
            typedef typename std::remove_reference<ref_t>::type const_key_t;
            typedef typename std::remove_cv<const_key_t>::type key_t;
            return std::hash<key_t>()(x->key());
        }
    }
};

template<typename T, int key_seq=0>struct subkey_hash{
public:
    size_t operator()(const T& x)const
    {
        typedef decltype(std::get<key_seq>(x->key())) ref_t;
        typedef typename std::remove_reference<ref_t>::type const_key_t;
        typedef typename std::remove_cv<const_key_t>::type key_t;
        if(is_null(x))
            return 0;
        else
            return std::hash<key_t>()(std::get<key_seq>(x->key()));
    }
};

};

using namespace std::rel_ops;

#endif // PROTON_REF_HEADER
