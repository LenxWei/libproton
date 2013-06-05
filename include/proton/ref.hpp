#ifndef PROTON_REF_HEADER
#define PROTON_REF_HEADER

/** @file ref.hpp
 *  @brief the core header for reference support.
 */

#include <memory>
#include <utility>
#include <tuple>
#include <type_traits>
#include <stdexcept>
#include <atomic>
#include <initializer_list>
#include <proton/pool.hpp>

#ifndef PROTON_REF_DEBUG
#define PROTON_REF_LOG(lvl, out)
#else
#define PROTON_REF_LOG PROTON_LOG
#endif

namespace proton{

namespace detail{

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

    static constexpr long weak_count()
    {
    	return 0;
    }
};

class wrefc_t {
private:
    long __r;
    long __w;

public:
    wrefc_t():__r(0),__w(0)
    {}

    wrefc_t(const wrefc_t& r):__r(0),__w(0)
    {}

    wrefc_t& operator=(const wrefc_t& r)
    {
        return *this;
    }

    void enter()
    {
        ++__r;
    }

    long release()
    {
        return --__r;
    }

    void weak_enter()
    {
        ++__w;
    }

    long weak_release()
    {
        return --__w;
    }

    long count() const
    {
        return __r;
    }

    long weak_count() const
    {
        return __w;
    }

};

} // ns detail

/** @addtogroup ref
 * @{
 * @addtogroup ref_
 * @{
 */

// [TODO] sref_: a simpler ref template for single-rooted class hierarchies.

class init_alloc{};
extern init_alloc alloc; ///< explicitly demand to initialize an object.

class init_alloc_inner{};
extern init_alloc_inner alloc_inner; //< for inner use of ref_.

class init_alloc_none{};
extern init_alloc_none none; ///< explicitly demand to construct an empty object.

enum ref_flags{
    ref_not_use_output=0x1,
    ref_immutable=0x2,
    ref_not_cast_obj=0x4
};

template<typename T>
struct ref_traits{
    static constexpr unsigned long long flag=0;
};

template<typename objT, typename allocator=smart_allocator<objT>,
		typename traits=ref_traits<objT>, typename refcT=detail::refc_t >
struct ref_;

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
template<typename O, typename A, typename T, typename R> ref_<O,A,T,R> copy(const ref_<O,A,T,R>& x)
{
    typedef ref_<O,A,T,R> refT;

    if(is_null(x))
        return refT();
    typedef typename refT::alloc_t alloc_t;
    typedef typename refT::refc_t refc_t;
    refc_t* p=(refc_t*)alloc_t::duplicate(x._rp);
    if(!p)
        throw std::bad_alloc();
    new (p) refc_t();
    typename refT::obj_t* q=(typename refT::obj_t *)(p+1);
    x->copy_to((void*)q);
    return refT(alloc_inner,p,q);
}


/** get the reference count of the object.
 * @param x refers to the object
 * @return the reference count.
 */
template<typename O, typename A, typename T, typename R> long ref_count(const ref_<O,A,T,R>& x)
{
    if(x._rp)
        return x._rp->count();
    else
        return 0;
}

/** cast from a ref type to another.
 * if casting fails, throw std::bad_cast().
 * @param x the original ref
 * @return the casted one
 */
template<typename C, typename O2, typename A2, typename T2, typename R2 >
C cast(const ref_<O2,A2,T2,R2>& x)
{
    if(x==none)
        return C();
    typedef typename C::obj_t target_t;
    target_t* p=dynamic_cast<target_t*>(x._p);
    if(p)
        return C(alloc_inner, x._rp, p);
    throw std::bad_cast();
}

/** The core reference support template.
 * @param allocator It must support confiscate(), and allocator::allocate() must be static.
 * @see smart_allocator in <proton/pool.hpp>
 */
template<typename objT, typename allocator, typename traits, typename refcT>
struct ref_ {
template<typename O, typename A, typename T, typename R>
    friend ref_<O,A,T,R> copy(const ref_<O,A,T,R>& x);
template<typename O, typename A, typename T, typename R>
    friend long ref_count(const ref_<O,A,T,R>& x);
template<typename C, typename O2, typename A2, typename T2, typename R >
    friend C cast(const ref_<O2,A2,T2, R>& x);
template<typename T>
	friend class weak_;

public:
    typedef ref_ proton_ref_self_t;
    typedef std::ostream proton_ostream_t;
    typedef std::wostream proton_wostream_t;
    typedef objT obj_t;
    typedef refcT refc_t;
    typedef allocator alloc_t;

protected:
    refc_t * _rp;
    objT*    _p;

    struct ref_obj_t{
        refc_t r;
        obj_t o;
    };
    typedef typename alloc_t::template rebind<ref_obj_t>::other real_alloc;

protected:
    void enter(refc_t* rp)
    {
        _rp=rp;
        if(_rp)
            _rp->enter();
    }

    void release()
    {
        if(_rp){
            if(!_rp->release()){
                _p->~objT();
                if(!_rp->weak_count()){
                	alloc_t::confiscate(_rp);
                }
            }
        	_rp=NULL;
            _p=NULL;
        }
    }

    void swap(ref_& r)
    {
    	refc_t* t_rp=r._rp;
    	obj_t* t_p=r._p;

    	r._rp=_rp;
    	r._p=_p;

    	_rp=t_rp;
    	_p=t_p;
    }

public:
    /** default ctor.
     * Doesn't refer to any object.
     */
    ref_():_rp(NULL), _p(NULL)
    {
        PROTON_REF_LOG(9,"default ctor");
    }

    ref_(init_alloc_none):_rp(NULL), _p(NULL)
    {
        PROTON_REF_LOG(9,"default ctor");
    }

    // inner use
    ref_(init_alloc_inner, refc_t* rp, objT* p):_rp(rp), _p(p)
    {
        PROTON_REF_LOG(9,"alloc_inner ctor");
        if(_rp)
            _rp->enter();
    }

    /** explicit forwarding ctor.
     * Construct an obj_t using give args.
     */
    template<typename ...argT> explicit ref_(init_alloc, argT&& ...a)
    {
        PROTON_REF_LOG(9,"alloc fwd ctor");
        ref_obj_t* p=real_alloc::allocate(1);
        if(p){
            new (&(p->r)) refc_t();
            new (&p->o) obj_t(a...); // [FIXME] how about exceptions?
            _p=&(p->o);
            enter(&(p->r));
        }
        else
            throw std::bad_alloc();
    }

    /** implicit forwarding ctor.
     * Construct an obj_t using give args.
     * Note: don't conflict with copy ctors. Use the explicit fwd ctor in that case.
     */
    template<typename ...argT> explicit ref_(argT&& ...a):ref_(alloc, a...)
    {}

	template<typename=typename std::enable_if<
            !(traits::flag & ref_not_cast_obj)
        >::type
        >
        ref_(const obj_t& a):ref_(alloc, a)
    {}

	template<typename=typename std::enable_if<
            !(traits::flag & ref_not_cast_obj)
        >::type
        >
        ref_(obj_t&& a):ref_(alloc, a)
    {}

    /** initializer_list forwarding ctor.
     * Construct an obj_t using give args.
     */
    template<typename T> ref_(init_alloc, std::initializer_list<T> a)
    {
        PROTON_REF_LOG(9,"alloc initializer_list fwd ctor");
        ref_obj_t* p=real_alloc::allocate(1);
        if(p){
            new (&(p->r)) refc_t();
            new (&p->o) obj_t(a);
            _p=&(p->o);
            enter(&(p->r));
        }
        else
            throw std::bad_alloc();
    }

    /** implicit initializer_list forwarding ctor.
     * Construct an obj_t using give args.
     */
    template<typename T> ref_(std::initializer_list<T> a):ref_(alloc,a)
    {}

    /** copy ctor.
     */
    ref_(const ref_& r):_p(r._p)
    {
        PROTON_REF_LOG(9,"const copy ctor");
        enter(r._rp);
    }

    ref_(ref_& r):_p(r._p)
    {
        PROTON_REF_LOG(9,"copy ctor");
        enter(r._rp);
    }

    ref_(const ref_&& r):_p(r._p)
    {
        PROTON_REF_LOG(9,"copy rvalue ctor");
        enter(r._rp);
    }

    /** move ctor.
     */
    ref_(ref_&& r)noexcept:_rp(r._rp),_p(r._p)
    {
        PROTON_REF_LOG(9,"move ctor");
        r._rp=NULL;
        r._p=NULL;
    }

    /** assign operator.
     */
    ref_& operator=(const ref_& r)
    {
        PROTON_REF_LOG(9,"assign lvalue");
        if(r._rp!=_rp){
        	ref_ r1(none);
        	swap(r1);

            enter(r._rp);
            _p=r._p;
        }
        return *this;
    }

    /** assign move operator.
     */
    ref_& operator=(ref_&& r)noexcept(noexcept(_p->~objT()))
    {
        PROTON_REF_LOG(9,"assign rvalue");
        if(r._rp!=_rp){
        	swap(r);
        }
        return *this;
    }

    ref_& operator=(init_alloc_none)noexcept(noexcept(_p->~objT()))
    {
        release();
        return *this;
    }

    template<typename T,
        typename=typename std::enable_if<std::is_pod<T>::value>::type
    >
    ref_& operator=(T a)
    {
        PROTON_REF_LOG(9,"assign argument");
        ref_obj_t* p=real_alloc::allocate(1);
        if(!p)
            throw std::bad_alloc();
        {
            new (&(p->r)) refc_t();
            new (&p->o) obj_t(a);

            ref_ r(none);
            swap(r);

            enter(&(p->r));
            _p=&(p->o);
        }
        return *this;
    }

    /** dtor.
     */
    ~ref_()noexcept(noexcept(_p->~objT()))
    {
        release();
    }

public:

#if 0
    /* conversion to const baseT&.
     * Notice! NEVER convert to a non-const ref from here!
     * Due to the optimizing ability of c++11, in most situations this method is not needed.
     * Use the next method for casting.
     */
    template<typename baseT> operator const baseT& () const
	{
        PROTON_REF_LOG(9,"const baseT&()");
		static_assert(std::is_class<typename baseT::proton_ref_self_t>(), "The target type is not a ref_ type");
		static_assert(std::is_base_of<typename baseT::obj_t, obj_t>(), "The target type is not a base type of obj_t");
		static_assert(static_cast<typename baseT::obj_t*>((obj_t*)4096)==(typename baseT::obj_t*)4096, "can not convert to a non-first-base ref_");
		return reinterpret_cast<const baseT&>(*this);
	}
#endif

#if 0
	template<typename derivedT,
        typename=typename std::enable_if<
            std::is_base_of<obj_t, derivedT>::value
        >::type
        >
        ref_(const ref_<derivedT>& x)noexcept:_rp(x._rp), _p(static_cast<obj_t*>(x._p))
    {
        if(_rp)
            _rp->enter();
    }
#endif

#if 0
	template<typename baseT,
        typename=typename std::enable_if<
            std::is_base_of<baseT, obj_t>::value
            && static_cast<baseT*>((obj_t*)4096)==(baseT*)4096
        >::type
        >
        operator const ref_<baseT>& ()const noexcept
    {
        PROTON_REF_LOG(9,"const baseT&()");
		return reinterpret_cast<const ref_<baseT>&>(*this);
	}
#endif

#if 1
    /** conversion to baseT.
     */
	template<typename baseT,
        typename=typename std::enable_if<
            std::is_base_of<baseT, obj_t>::value
//          && static_cast<baseT*>((obj_t*)4096)!=(baseT*)4096
        >::type
        >
        operator ref_<baseT, smart_allocator<baseT>, ref_traits<baseT>, refc_t > ()const noexcept
    {
        PROTON_REF_LOG(9,"baseT()");
		return ref_<baseT, smart_allocator<baseT>, ref_traits<baseT>, refc_t>(alloc_inner, _rp, static_cast<baseT*>(_p));
	}

	template<typename=typename std::enable_if<
            !(traits::flag & ref_not_cast_obj)
        >::type
        >
        operator obj_t&()
    {
        return __o();
    }

	template<typename=typename std::enable_if<
            !(traits::flag & ref_not_cast_obj)
        >::type
        >
        operator const obj_t&()const
    {
        return __o();
    }

#endif

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

    /** operator-> points to the object refered.
     */
    objT* operator->()
    {
        return &__o();
    }

    /** operator-> points to the object refered.
     */
    const objT* operator->()const
    {
        return &__o();
    }

    /** x == none.
     * test whether x is empty.
     */
    bool operator==(const init_alloc_none&)const
    {
        return _p==NULL;
    }

    /** x != none.
     * test whether x is empty.
     */
    bool operator!=(const init_alloc_none&)const
    {
        return _p!=NULL;
    }

    /** general operator== for refs.
     * Need to implement obj_t == T::obj_t.
     */
    template<typename O, typename A, typename T, typename R> bool operator==(const ref_<O,A,T,R>& x)const
    {
        if((void*)&(__o())==(void*)&(x.__o()))
            return true;
        if(*this==none || x==none)
            return false;
        return __o() == x.__o();
    }

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, bool>::type
        operator==(const T& x)const
    {
        if(*this==none)
            return false;
        return __o() == x;
    }

    template<typename T> bool operator!=(const T& x)const
    {
        return !(*this==x);
    }

    /** general operator< for refs.
     * Need to implement obj_t < T::obj_t.
     */
    template<typename O, typename A, typename T, typename R>
    bool operator<(const ref_<O,A,T,R>& x)const
    {
        if((void*)&(x.__o())==(void*)&(__o()))
            return false;
        if(*this==none)
            return true;
        if(x==none)
            return false;
        return __o() < x.__o();
    }

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value, bool>::type
        operator<(const T& x)const
    {
        if(*this==none)
            return true;
        return __o() < x;
    }

    template<typename T> bool operator>=(const T& x)const
    {
        return !(*this < x);
    }

    template<typename T> bool operator>(const T& x)const
    {
        return (x < *this);
    }

    template<typename T> bool operator<=(const T& x)const
    {
        return !(x < *this);
    }

    /** general operator() const for refs.
     * Need to implement obj_t() const.
     */
    template<typename ...T> auto operator()(T&& ...x)const -> decltype((*_p)(x...))
    {
        PROTON_THROW_IF(*this==none, "nullptr for ()");
        return __o()(x...);
    }

    /** general operator() for refs.
     * Need to implement obj_t().
     */
    template<typename ...T> auto operator()(T&& ...x) -> decltype((*_p)(x...))
    {
        PROTON_THROW_IF(*this==none, "nullptr for ()");
        return __o()(x...);
    }

    /** general operator[] const for refs.
     * Need to implement obj_t[] const.
     */
    template<typename T> auto operator[](T&& x)const -> decltype((*_p)[x])
    {
        PROTON_THROW_IF(*this==none, "nullptr for []");
        return __o()[x];
    }

    /** general operator[] for refs.
     * Need to implement obj_t[].
     */
    template<typename T> auto operator[](T&& x) -> decltype((*_p)[x])
    {
        PROTON_THROW_IF(*this==none, "nullptr for []");
        return __o()[x];
    }

    /** ref_ + ref_
     * [TODO] using move ctor
     */
    template<typename T,
        typename=typename std::enable_if<std::is_same<T, ref_>::value>::type
        >
    ref_ operator+(const T& x)const
    {
        PROTON_THROW_IF(x==none || *this==none,"want to add null values");
        refc_t* p=(refc_t*)alloc_t::duplicate(_rp);
        if(!p)
            throw std::bad_alloc();
        new (p) refc_t();
        obj_t* q=(obj_t *)(p+1);
        new (q) obj_t(__o()+x.__o());
        return ref_(alloc_inner,p,q);
    }

    /** ref_ + pod
     */
    template<typename T,
        typename=typename std::enable_if<std::is_pod<T>::value>::type
        >
    ref_ operator+(T x)const
    {
        PROTON_THROW_IF(*this==none,"want to add null values");
        refc_t* p=(refc_t*)alloc_t::duplicate(_rp);
        if(!p)
            throw std::bad_alloc();
        new (p) refc_t();
        obj_t* q=(obj_t *)(p+1);
        new (q) obj_t(__o()+x);
        return ref_(alloc_inner,p,q);
    }

    /** ref_ * pod
     */
    template<typename T,
        typename=typename std::enable_if<std::is_pod<T>::value>::type
        >
    ref_ operator*(T x)const
    {
        PROTON_THROW_IF(*this==none,"want to * null values");
        refc_t* p=(refc_t*)alloc_t::duplicate(_rp);
        if(!p)
            throw std::bad_alloc();
        new (p) refc_t();
        obj_t* q=(obj_t *)(p+1);
        new (q) obj_t(__o()*x);
        return ref_(alloc_inner,p,q);
    }

    /** ref_ % other
     */
    template<typename T>
    ref_ operator%(const T& x)const
    {
        PROTON_THROW_IF(*this==none, "want to % null values");
        refc_t* p=(refc_t*)alloc_t::duplicate(_rp);
        if(!p)
            throw std::bad_alloc();
        new (p) refc_t();
        obj_t* q=(obj_t *)(p+1);
        new (q) obj_t(__o() % x);
        return ref_(alloc_inner,p,q);
    }

    /** ref_ << other
     */
    template<typename T>
    ref_ operator<<(const T& x)
    {
        PROTON_THROW_IF(*this==none, "want to << null values");
        __o() << x;
        return *this;
    }

    /** ref_ >> other
     */
    template<typename T>
    ref_ operator>>(const T& x)
    {
        PROTON_THROW_IF(*this==none, "want to >> null values");
        __o() >> x;
        return *this;
    }

    /** +=
     */
    template<typename T>
    typename std::enable_if<std::is_convertible<T, ref_>::value && !(traits::flag & ref_immutable),
        ref_&>::type
        operator+=(const T& x)
    {
        PROTON_THROW_IF(x==none || *this==none, "want to add null values");
        __o()+=x.__o();
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value && !(traits::flag & ref_immutable),
        ref_&>::type
        operator+=(T x)
    {
        PROTON_THROW_IF(*this==none,"want to add null values");
        __o()+=x;
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_convertible<T, ref_>::value && (traits::flag & ref_immutable),
        ref_&>::type
        operator+=(const T& x)
    {
        PROTON_THROW_IF(x==none || *this==none,"want to add null values");

        ref_obj_t* p=real_alloc::allocate(1);
        if(!p)
            throw std::bad_alloc();
        {
            new (&(p->r)) refc_t();
            new (&p->o) obj_t(__o()+x.__o());

            ref_ r(none);
            swap(r);

            enter(&(p->r));
            _p=&(p->o);
        }
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value && (traits::flag & ref_immutable),
        ref_&>::type
        operator+=(T x)
    {
        PROTON_THROW_IF(*this==none,"want to add null values");

        ref_obj_t* p=real_alloc::allocate(1);
        if(!p)
            throw std::bad_alloc();
        {
            new (&(p->r)) refc_t();
            new (&p->o) obj_t(__o()+x);

            ref_ r(none);
            swap(r);

            enter(&(p->r));
            _p=&(p->o);
        }
        return *this;
    }

    /** *=
     */
    template<typename T>
    typename std::enable_if<std::is_pod<T>::value && !(traits::flag & ref_immutable),
        ref_&>::type
        operator*=(T x)
    {
        PROTON_THROW_IF(*this==none,"want to *= null values");
        __o()*=x;
        return *this;
    }

    template<typename T>
    typename std::enable_if<std::is_pod<T>::value && (traits::flag & ref_immutable),
        ref_&>::type
        operator*=(T x)
    {
        PROTON_THROW_IF(*this==none,"want to *= null values");

        ref_obj_t* p=real_alloc::allocate(1);
        if(!p)
            throw std::bad_alloc();
        {
            new (&(p->r)) refc_t();
            new (&p->o) obj_t(__o()*x);

            ref_ r(none);
            swap(r);

            enter(&(p->r));
            _p=&(p->o);
        }
        return *this;
    }

};

/* [obsolete] use x==none
 * test a ref null or not.
 * @param x the ref to be checked
 * @return true: x doesn't refer to any object, false: x refers to an object.
 */
template<typename O, typename A, typename T, typename R> bool
is_null(const ref_<O,A,T,R>& x)
{
    return x==none;
}

template<typename O, typename A, typename T, typename R>
bool operator==(init_alloc_none,const ref_<O,A,T,R>& x)
{
    return x==none;
}

/* [obsolete] use x!=none
 * test a ref valid or not.
 * @param x the ref to be checked
 * @return true: x refers to an object, false: x doesn't refer to any object.
 */
template<typename O, typename A, typename T, typename R>
bool is_valid(const ref_<O,A,T,R>& x)
{
    return x!=none;
}

template<typename O, typename A, typename T, typename R>
bool operator!=(init_alloc_none,const ref_<O,A,T,R>& x)
{
    return x!=none;
}

/* [obsolete] use x=none;
 * reset a ref to release its object if any.
 * @param x the ref to be resetted.
 */
template<typename O, typename A, typename T, typename R>
void reset(ref_<O,A,T,R>& x)
{
    x=none;
}

/** general output for refs.
 * Need O to implenment the method: void output(std::ostream& s)const.
 * Don't forget virtual when needed.
 */
template<typename O, typename A, typename T, typename R>
typename std::enable_if<!(T::flag & ref_not_use_output), std::ostream&>::type
operator<<(std::ostream& s, const ref_<O,A,T,R>& y)
{
    if(y==none){
        s << "<>" ;
        return s;
    }
    y->output(s);
    return s;
}

/** general output for refs.
 * Need to support s << O
 */
template<typename O, typename A, typename T, typename R>
typename std::enable_if<T::flag & ref_not_use_output, std::ostream&>::type
operator<<(std::ostream& s, const ref_<O,A,T,R>& y)
{
    if(y==none){
        s << "<>" ;
        return s;
    }
    s << y.__o();
    return s;
}

/** general wchar output for refs.
 * Need O to implenment the method: void output(std::ostream& s)const.
 * Don't forget virtual when needed.
 */
template<typename O, typename A, typename T, typename R>
typename std::enable_if<!(T::flag & ref_not_use_output), std::wostream&>::type
operator<<(std::wostream& s, const ref_<O,A,T,R>& y)
{
    if(y==none){
        s << L"<>" ;
        return s;
    }
    y->output(s);
    return s;
}

/** general wchar output for refs.
 * Need to support s << O
 */
template<typename O, typename A, typename T, typename R>
typename std::enable_if<(T::flag & ref_not_use_output), std::wostream&>::type
operator<<(std::wostream& s, const ref_<O,A,T,R>& y)
{
    if(y==none){
        s << L"<>" ;
        return s;
    }
    s << y;
    return s;
}

/** general operator< & operator== for objects.
 * Need obj_t to implenment: T1 key()const, and T1 should be comparable.
 * Don't forget virtual when needed.
 * [TODO] need an example.
 */
#define PROTON_KEY_DECL(type)\
    bool operator<(const type& y)const\
    {\
        return key()<y.key();\
    }\
    \
    bool operator==(const type& y)const\
    {\
        return key()==y.key();\
    }\

/** general key_hash for refs.
 * Need T::obj_t to implenment T1 key()const, and T1 must support std::hash.
 * Don't forget virtual when needed.
 * [TODO] need an example.
 */
template<typename T>struct key_hash{
public:
    size_t operator()(const T& x)const
    {
        if(x==none)
            return 0;
        else{
            typedef decltype(x->key()) ref_t;
            typedef typename std::remove_reference<ref_t>::type const_key_t;
            typedef typename std::remove_cv<const_key_t>::type key_t;
            return std::hash<key_t>()(x->key());
        }
    }
};

/** general subkey_hash for refs.
 * Need T::obj_t to implenment T1 key()const, and T1 is a tuple,
 * and the key_seq item of T1 must support std::hash.
 * Don't forget virtual when needed.
 * [TODO] need an example.
 */
template<typename T, int key_seq=0>struct subkey_hash{
public:
    size_t operator()(const T& x)const
    {
        typedef decltype(std::get<key_seq>(x->key())) ref_t;
        typedef typename std::remove_reference<ref_t>::type const_key_t;
        typedef typename std::remove_cv<const_key_t>::type key_t;
        if(x==none)
            return 0;
        else
            return std::hash<key_t>()(std::get<key_seq>(x->key()));
    }
};

/**
 * @}
 * @}
 */

namespace detail{

template<typename O, typename A, typename T, typename R>struct len_t<ref_<O,A,T,R> >{
//    typedef decltype(*(((T*)1)->begin())) item_t;
    static size_t result(const ref_<O,A,T,R>& x)
    {
        PROTON_THROW_IF(x==none, "no len() for an empty object");
        return x->size();
    }
};

} // ns detail

} // ns proton

namespace std{

template<typename O, typename A, typename T, typename R>
inline auto begin(const proton::ref_<O,A,T,R>& a) -> decltype(a->begin())
{
    return a->begin();
}

template<typename O, typename A, typename T, typename R>
inline auto end(const proton::ref_<O,A,T,R>& a) -> decltype(a->end())
{
    return a->end();
}

template<typename O, typename A, typename T, typename R>
struct hash<proton::ref_<O,A,T,R> >{
public:
    typedef size_t     result_type;
    typedef proton::ref_<O,A,T,R>      argument_type;
    inline size_t operator()(const proton::ref_<O,A,T,R> &s) const noexcept
    {
        if(s==proton::none)
            return 0;
        return std::hash<O>()(s.__o());
    }
};

} // ns std


#endif // PROTON_REF_HEADER
