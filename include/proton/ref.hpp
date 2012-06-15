#ifndef PROTON_REF_HEADER
#define PROTON_REF_HEADER

/** @file ref.hpp
 *  @brief the core header for reference support.
 */

#include <memory>
#include <utility>
#include <tuple>
#include <type_traits>
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

} // ns detail

/** @defgroup ref Smart reference
 * Provide core reference support for proton.
 * @{
 */

class init_alloc{};
extern init_alloc alloc; ///< explicitly demand to initialize an object.

class init_alloc_inner{};
extern init_alloc_inner alloc_inner; //< for inner use of ref_.

/** test a ref null or not.
 * @param x the ref to be checked
 * @return true: x doesn't refer to any object, false: x refers to an object.
 */
template<typename refT> bool is_null(const refT& x)
{
    return &x.__o()==NULL;
}

/** test a ref valid or not.
 * @param x the ref to be checked
 * @return true: x refers to an object, false: x doesn't refer to any object.
 */
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
    detail::refc_t* p=(detail::refc_t*)alloc_t::duplicate(x._rp);
    new (p) detail::refc_t();
    typename refT::obj_t* q=(typename refT::obj_t *)(p+1);
    x->copy_to((void*)q);
    return refT(alloc_inner,p,q);
}

/** reset a ref to release its object if any.
 * @param x the ref to be resetted.
 */
template<typename refT> void reset(refT& x)
{
    x.release();
}

/** get the reference count of the object.
 * @param x refers to the object
 * @return the reference count.
 */
template<typename refT> long ref_count(const refT& x)
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
template<typename T, typename refT> T cast(const refT& x)
{
    static_assert(std::is_class<typename T::proton_ref_self_t>(), "The target type is not a ref_ type");
    typedef typename T::obj_t target_t;
    if(std::is_base_of<target_t, typename refT::obj_t>())
        return T(alloc_inner, x._rp, static_cast<target_t*>(x._p));
    else{
        target_t* p=dynamic_cast<target_t*>(x._p);
        if(p)
            return T(alloc_inner, x._rp, p);
        else
            throw std::bad_cast();
    }
}

/** The core reference support template.
 * @param allocator It must support confiscate(), and allocator::allocate() must be static.
 * @see smart_allocator in <proton/pool.hpp>
 */
template<typename objT, typename allocator=smart_allocator<objT> > struct ref_ {
friend void reset<ref_>(ref_& x);
friend ref_ copy<ref_>(const ref_& x);
friend long ref_count<ref_>(const ref_& x);
template<typename T, typename ref_>friend  T cast(const ref_& x);

public:
    typedef ref_ proton_ref_self_t;
    typedef std::ostream proton_ostream_t;
    typedef objT obj_t;
    typedef allocator alloc_t;

protected:
    detail::refc_t * _rp;
    objT*    _p;

protected:
    void enter(detail::refc_t* rp)
    {
        _rp=rp;
        if(_rp)
            _rp->enter();
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
    /** default ctor.
     * Doesn't refer to any object.
     */
    ref_():_rp(NULL), _p(NULL)
    {
        PROTON_REF_LOG(9,"default ctor");
    }

    // inner use
    ref_(init_alloc_inner, detail::refc_t* rp, objT* p):_rp(rp), _p(p)
    {
        PROTON_REF_LOG(9,"alloc_inner ctor");
        if(_rp)
            _rp->enter();
    }

    template<typename ...argT> explicit ref_(init_alloc, argT&& ...a)
    {
        PROTON_REF_LOG(9,"alloc fwd ctor");
        struct ref_obj_t{
            detail::refc_t r;
            obj_t o;
        };
        typedef typename alloc_t::template rebind<ref_obj_t>::other real_alloc;
        ref_obj_t* p=real_alloc::allocate(1);
        if(p){
            new (&(p->r)) detail::refc_t();
            new (&p->o) obj_t(a...);
            _p=&(p->o);
            enter(&(p->r));
        }
    }

    template<typename ...argT> explicit ref_(argT&& ...a)
    {
        PROTON_REF_LOG(9,"fwd ctor");
        struct ref_obj_t{
            detail::refc_t r;
            obj_t o;
        };
        typedef typename alloc_t::template rebind<ref_obj_t>::other real_alloc;
        ref_obj_t* p=real_alloc::allocate(1);
        if(p){
            new (&(p->r)) detail::refc_t();
            new (&(p->o)) obj_t(a...);
            _p=&(p->o);
            enter(&(p->r));
        }
    }

    /** const copy ctor.
     */
    ref_(const ref_& r):_p(r._p)
    {
        PROTON_REF_LOG(9,"const copy ctor");
        enter(r._rp);
    }

    /** copy ctor.
     * gcc bug? without this, there will be compile errors.
     */
    ref_(ref_& r):_p(r._p)
    {
        PROTON_REF_LOG(9,"copy ctor");
        enter(r._rp);
    }

    /** move ctor.
     */
    ref_(ref_&& r):_rp(r._rp),_p(r._p)
    {
        PROTON_REF_LOG(9,"move ctor");
        r._rp=NULL;
        r._p=NULL;
    }

    /** assign operator.
     */
    ref_& operator=(ref_ r)
    {
        PROTON_REF_LOG(9,"assign");
        if(r._rp!=_rp){
            detail::refc_t* rp_old=_rp;
            objT* p_old=_p;

            _rp=r._rp;
            _p=r._p;
            r._rp=NULL;
            r._p=NULL;

            if(rp_old){
                long r=rp_old->release();
                if(!r){
                    p_old->~objT(); // may throw
                    alloc_t::confiscate(rp_old);
                }
            }
        }
        return *this;
    }

    /** dtor.
     */
    ~ref_()
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

    /** conversion to baseT.
     */
	template<typename baseT> operator baseT () const
	{
        PROTON_REF_LOG(9,"baseT()");
		static_assert(std::is_class<typename baseT::proton_ref_self_t>(), "The target type is not a ref_ type");
		static_assert(std::is_base_of<typename baseT::obj_t, obj_t>(), "The target type is not a base type of obj_t");
		return baseT(alloc_inner, _rp, static_cast<typename baseT::obj_t*>(_p));
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

    /** general operator== for refs.
     * Need to implement obj_t == T::obj_t.
     */
    template<typename T> bool operator==(const T& x)const
    {
        static_assert(std::is_class<typename T::proton_ref_self_t>(),
                      "The target type is not a ref_ type");
        if((void*)&(__o())==(void*)&(x.__o()))
            return true;
        if(is_null(*this)||is_null(x))
            return false;
        return __o() == x.__o();
    }

    template<typename T> bool operator!=(const T& x)const
    {
        return !(*this==x);
    }

    /** general operator< for refs.
     * Need to implement obj_t < T::obj_t.
     */
    template<typename T> bool operator<(const T& x)const
    {
        static_assert(std::is_class<typename T::proton_ref_self_t>(),
                      "The target type is not a ref_ type");
        if((void*)&(x.__o())==(void*)&(__o()))
            return false;
        if(is_null(*this))
            return true;
        if(is_null(x))
            return false;
        return __o() < x.__o();
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
        PROTON_THROW_IF(is_null(*this), "nullptr for ()");
        return __o()(x...);
    }

    /** general operator() for refs.
     * Need to implement obj_t().
     */
    template<typename ...T> auto operator()(T&& ...x) -> decltype((*_p)(x...))
    {
        PROTON_THROW_IF(is_null(*this), "nullptr for ()");
        return __o()(x...);
    }

    /** general operator[] const for refs.
     * Need to implement obj_t[] const.
     */
    template<typename T> auto operator[](T&& x)const -> decltype((*_p)[x])
    {
        PROTON_THROW_IF(is_null(*this), "nullptr for []");
        return __o()[x];
    }

    /** general operator[] for refs.
     * Need to implement obj_t[].
     */
    template<typename T> auto operator[](T&& x) -> decltype((*_p)[x])
    {
        PROTON_THROW_IF(is_null(*this), "nullptr for []");
        return __o()[x];
    }
};

/** general output for refs.
 * Need T::obj_t to implenment the method: void output(std::ostream& s)const.
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
        if(is_null(x))
            return 0;
        else
            return std::hash<key_t>()(std::get<key_seq>(x->key()));
    }
};

/**
 * @}
 */
};

#endif // PROTON_REF_HEADER
