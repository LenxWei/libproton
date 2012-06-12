#ifndef PROTON_REF_HEADER
#define PROTON_REF_HEADER

/** @file ref.hpp
 *  @brief the core header for reference support.
 */

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
    return &x.__o()==NULL;
}

template<typename refT> bool is_valid(const refT& x)
{
    return &x.__o()!=NULL;
}

/** declare copy_to().
 * For object classes which need to support copy().
 */
#define PROTON_COPY_DECL(type)\
    virtual void copy_to(void* p)const\
    {\
        new (p) type(*this);\
    }

/** declare copy_to() without virtual.
 * For object classes which need to support copy(), without inheritance.
 */
#define PROTON_COPY_DECL_NV(type)\
    void copy_to(void* p)const\
    {\
        new (p) type(*this);\
    }

/** Generate a copy of object.
 * Note: the alloc_t of refT must support duplicate() like smart_allocator.
 * @param x a ref to an obj supporting the method: void copy_to(void* new_addr)const.
 *          You can use PROTON_COPY_DECL() or PROTON_COPY_DECL_NV() to declare copy_to()
 *          in the obj class.
 * @return a cloned obj of x
 */
template<typename refT> refT copy(const refT& x)
{
    if(is_null(x))
        return refT();
    typedef typename refT::alloc_t alloc_t;
    refc_t* p=(refc_t*)alloc_t::duplicate(x._rp);
    new (p) refc_t();
    typename refT::obj_t* q=(typename refT::obj_t *)(p+1);
    x->copy_to((void*)q);
    return refT(alloc,p,q);
}

template<typename refT> void reset(refT& x)
{
    x.release();
}

template<typename refT> long ref_count(const refT& x)
{
    if(x._rp)
        return x._rp->count();
    else
        return 0;
}

template<typename T, typename refT> T cast(const refT& x)
{
    static_assert(std::is_class<typename T::proton_ref_self_t>(), "The target type is not a ref_ type");
    typedef typename T::obj_t target_t;
    if(std::is_base_of<target_t, typename refT::obj_t>())
        return T(alloc, x._rp, static_cast<target_t*>(x._p));
    else{
        target_t* p=dynamic_cast<target_t*>(x._p);
        if(p)
            return T(alloc, x._rp, p);
        else
            throw std::bad_cast();
    }
}

/** The core reference support template.
 * @param allocator must support confiscate(), see smart_allocator in <proton/pool.hpp>
 */
template<typename objT, typename allocator=smart_allocator<objT> > struct ref_ {
friend void reset<ref_>(ref_& x);
friend ref_ copy<ref_>(const ref_& x);
friend long ref_count<ref_>(const ref_& x);

public:
    typedef ref_ proton_ref_self_t;
    typedef std::ostream proton_ostream_t;
    typedef objT obj_t;
    typedef allocator alloc_t;

protected:
    refc_t * _rp;
    objT*    _p;

protected:
    void enter(refc_t* rp)
    {
        _rp=rp;
        if(_rp)
            _rp->enter();
    }

    void assign(refc_t* rp, objT* p)
    {
        if(rp!=_rp){
            refc_t* rp_old=_rp;
            objT* p_old=_p;

            enter(rp);
            _p=p;

            if(rp_old){
                long r=rp_old->release();
                if(!r){
                    p_old->~objT(); // may throw
                    alloc_t::confiscate(rp_old);
                }
            }
        }
    }

    void release()
    {
        if(_rp){
            long r=_rp->release();
            if(!r){
                _p->~objT();
                alloc_t::confiscate(_rp);
            }
            _rp=NULL;
            _p=NULL;
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
            _p=&(p->o);
            enter(&(p->r));
        }
    }

    ref_(const ref_& r):_p(r._p)
    {
        enter(r._rp);
    }

    ref_(ref_& r):_p(r._p)
    {
        enter(r._rp);
    }

    ref_(ref_&& r):_p(r._p)
    {
        _rp=r._rp;
        r._rp=NULL;
    }

    ref_& operator=(const ref_& r)
    {
        assign(r._rp,r._p);
        return *this;
    }

    ~ref_()
    {
        release();
    }

public:
    /** conversion to const baseT&.
     * Notice! NEVER convert to a non-const ref from here!
     */
    template<typename baseT> operator const baseT& () const
	{
		static_assert(std::is_class<typename baseT::proton_ref_self_t>(), "The target type is not a ref_ type");
		static_assert(std::is_base_of<typename baseT::obj_t, obj_t>(), "The target type is not a base type of obj_t");
		static_assert(static_cast<typename baseT::obj_t*>((obj_t*)4096)==(typename baseT::obj_t*)4096, "can not convert to a non-first-base ref_");
		return reinterpret_cast<const baseT&>(*this);
	}

    /** conversion to baseT.
     */
	template<typename baseT> operator baseT () const
	{
		static_assert(std::is_class<typename baseT::proton_ref_self_t>(), "The target type is not a ref_ type");
		static_assert(std::is_base_of<typename baseT::obj_t, obj_t>(), "The target type is not a base type of obj_t");
		return baseT(alloc, _rp, static_cast<typename baseT::obj_t*>(_p));
	}

public:
    const objT& __o()const
    {
        return *_p;
    }

    objT& __o()
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
