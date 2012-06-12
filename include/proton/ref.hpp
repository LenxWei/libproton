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

    void enter()
    {
        ++__r;
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
 * Note: the alloc_t of refT must support duplicate() like smart_allocator.
 * @param x an obj supporting the method: void copy_to(void* new_addr)const
 * @return a cloned obj of x
 */
template<typename refT> refT copy(const refT& x)
{
    if(is_null(x))
        return refT();
    typedef typename refT::alloc_t alloc_t;
    refc_t* p=(refc_t*)alloc_t::duplicate(x.__rp());
    new (p) refc_t();
    typename refT::obj_t* q=(typename refT::obj_t *)(p+1);
    x->copy_to((void*)q);
    return refT(alloc,p,q);
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
friend void reset<ref_>(ref_& x);

public:
    typedef ref_ proton_ref_self_t;
    typedef std::ostream proton_ostream_t;
    typedef objT obj_t;
    typedef allocator alloc_t;

protected:
    refc_t * _rp;
    objT*    _p;

protected:
    void enter(refc_t* p)
    {
        _rp=p;
        if(_rp)
            _rp->enter();
    }

    void assign(refc_t* p)
    {
        if(p!=_rp){
            // [TODO] change to swap mechanism
            __release();
            enter(p);
        }
    }

    void __release()
    {
        if(_rp){
            int r=_rp->release();
            if(!r){
                _p->~objT();
                alloc_t::confiscate(_rp);
            }
            _rp=NULL;
        }
    }

public:
    // ctors
    ref_():_rp(NULL), _p(NULL)
    {}

    ref_(init_alloc, refc_t* rp, objT* p):_rp(rp), _p(p)
    {
        if(_rp)
            _rp->enter();
    }

    template<typename ...argT> explicit ref_(argT&& ...a)
    {
        struct ref_obj_t{
            refc_t r;
            obj_t o;
        };
        typedef typename alloc_t::template rebind<ref_obj_t>::other real_alloc;
        ref_obj_t* p=real_alloc::allocate(1);
        if(p){
            new (&(p->r)) refc_t();
            new (&(p->o)) obj_t(a...);
            _p=&(p->o);
            enter(&(p->r));
        }
    }

    template<typename ...argT> explicit ref_(init_alloc, argT&& ...a)
    {
        struct ref_obj_t{
            refc_t r;
            obj_t o;
        };
        typedef typename alloc_t::template rebind<ref_obj_t>::other real_alloc;
        ref_obj_t* p=real_alloc::allocate(1);
        if(p){
            new (&(p->r)) refc_t();
            new (&p->o) obj_t(a...);
            enter(&(p->r));
        }
    }

    ref_(const ref_& r):_p(r._p)
    {
        enter(r.__rp());
    }

    ref_(ref_& r):_p(r._p)
    {
        enter(r.__rp());
    }

    ref_(ref_&& r):_p(r._p)
    {
        _rp=r._rp;
        r._rp=NULL;
    }

    ref_& operator=(const ref_& r)
    {
        _p=r._p;
        assign(r.__rp());
        return *this;
    }

    ~ref_()
    {
        __release();
    }

public:
    /** conversion to const baseT&.
     * Notice! NEVER convert to a non-const ref!
     */
    template<typename baseT> operator const baseT& () const
	{
		static_assert(std::is_class<typename baseT::obj_t>(), "The target type is not a ref_ type");
		static_assert(std::is_base_of<typename baseT::obj_t, obj_t>(), "The target type is not a base type of obj_t");
		static_assert(static_cast<typename baseT::obj_t*>((obj_t*)4096)==(typename baseT::obj_t*)4096, "can not convert to a non-first-base ref_");
		return reinterpret_cast<const baseT&>(*this);
	}

    /** conversion to baseT.
     */
	template<typename baseT> operator baseT () const
	{
		static_assert(std::is_class<typename baseT::obj_t>(), "The target type is not a ref_ type");
		static_assert(std::is_base_of<typename baseT::obj_t, obj_t>(), "The target type is not a base type of obj_t");
		return baseT(alloc, _rp, _p);
	}

public:
    objT& __o()
    {
        return *_p;
    }

    const objT& __o()const
    {
        return *_p;
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
        return _p;
    }

    const objT* __p()const
    {
        return _p;
    }

    refc_t* __rp()const
    {
        return _rp;
    }

};

/** general output for ref_<> objects.
 * Need obj_t to implenment the method: void output(std::ostream& s)const.
 * Don't forget virtual when needed.
 */
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
