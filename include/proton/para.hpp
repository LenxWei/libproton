/*
 * para.hpp
 *
 *  Created on: Jul 28, 2013
 *      Author: lenx
 */

#ifndef PROTON_PARA_HPP_
#define PROTON_PARA_HPP_

#include <proton/base.hpp>
#include <proton/ref.hpp>

namespace proton{

template<typename refT>
class para_ {

template<typename O, typename A, typename T, typename R>
	friend long ref_count(const ref_<O,A,T,R>& x);
template<typename C, typename O2, typename A2, typename T2, typename R >
	friend C cast(const ref_<O2,A2,T2, R>& x);

public:
	typedef typename refT ref_t;
	typedef typename refT::refc_t refc_t;
	static_assert(std::is_pod<typename refc_t::support_weak>::value, "refT must be a weak-compatible class.");

	typedef typename refT::obj_t obj_t;
	typedef typename refT::alloc_t alloc_t;
	typedef typename refT::traits traits;

	typedef ref_t proton_ref_self_t;
	typedef std::ostream proton_ostream_t;
	typedef std::wostream proton_wostream_t;

private:

	refc_t* _w;
	obj_t* _p;

protected:

    void enter(refc_t* w)
    {
        _w=w;
        if(_w)
            _w->weak_enter();
    }

    void release()
    {
        if(_w){
            if(!_w->weak_release()){
                if(!_w->count()){
                	alloc_t::confiscate(_rp);
                }
            }
        	_w=NULL;
            _p=NULL;
        }
    }

    void swap(para_& p)
    {
    	// [todo: optimization]
    	refc_t* t_w=p._w;
    	obj_t* t_p=p._p;

    	p._w=_w;
    	p._p=_p;

    	_rp=t_w;
    	_p=t_p;
    }

public:

	para_(const refT& r):_w(r._rp), _p(r._p)
	{
		if(_w)
			_w->weak_enter();
	}

protected:
    // inner use
    para_(init_alloc_inner, refc_t* w, objT* p):_w(w), _p(p)
    {
        PROTON_REF_LOG(9,"alloc_inner ctor");
        if(_w)
            _w->weak_enter();
    }

public:
    /** default ctor.
     * Doesn't refer to any object.
     */
    para_():_w(NULL), _p(NULL)
    {
        PROTON_REF_LOG(9,"default ctor");
    }

    para_(init_alloc_none):_w(NULL), _p(NULL)
    {
        PROTON_REF_LOG(9,"default ctor");
    }

    /** copy ctor.
     */
    para_(const para_& p):_p(p._p)
    {
        PROTON_REF_LOG(9,"const copy ctor");
        enter(p._w);
    }

    para_(para_& p):_p(p._p)
    {
        PROTON_REF_LOG(9,"copy ctor");
        enter(p._w);
    }

    para_(const para_&& p):_p(p._p)
    {
        PROTON_REF_LOG(9,"copy rvalue ctor");
        enter(p._w);
    }

    /** move ctor.
     */
    para_(para_&& r)noexcept:_w(r._w),_p(r._p)
    {
        PROTON_REF_LOG(9,"move ctor");
        r._w=NULL;
        r._p=NULL;
    }

    /** assign operator.
     */
    para_& operator=(const para_& r)
    {
        PROTON_REF_LOG(9,"assign lvalue");
        if(r._w!=_w){
        	para_ r1(none);
        	swap(r1);

            enter(r._w);
            _p=r._p;
        }
        else{
        	_p=r._p;
        }
        return *this;
    }

    /** assign move operator.
     */
    para_& operator=(para_&& r)noexcept
    {
        PROTON_REF_LOG(9,"assign rvalue");
        if(r._w!=_w){
        	swap(r);
        }
        else{
        	_p=r._p;
        }
        return *this;
    }

    para_& operator=(init_alloc_none)noexcept
    {
        release();
        return *this;
    }

    /** dtor.
     */
    ~para_()noexcept
    {
        release();
    }

public:


#if 1
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
    template<typename T>
    typename std::enable_if<std::is_class<typename T::proton_ref_self_t>::value, bool>::type
		operator==(const T& x)const
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
    template<typename T>
    typename std::enable_if<std::is_class<typename T::proton_ref_self_t>::value, bool>::type
		operator<(const T& x)const
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
    ref_t operator+(const T& x)const
    {
        PROTON_THROW_IF(x==none || *this==none,"want to add null values");
        refc_t* p=(refc_t*)alloc_t::duplicate(_rp);
        if(!p)
            throw std::bad_alloc();
        new (p) refc_t();
        obj_t* q=(obj_t *)(p+1);
        new (q) obj_t(__o()+x.__o());
        return ref_t(alloc_inner,p,q);
    }

    /** ref_ + pod
     */
    template<typename T,
        typename=typename std::enable_if<std::is_pod<T>::value>::type
        >
    ref_t operator+(T x)const
    {
        PROTON_THROW_IF(*this==none,"want to add null values");
        refc_t* p=(refc_t*)alloc_t::duplicate(_rp);
        if(!p)
            throw std::bad_alloc();
        new (p) refc_t();
        obj_t* q=(obj_t *)(p+1);
        new (q) obj_t(__o()+x);
        return ref_t(alloc_inner,p,q);
    }

    /** ref_ * pod
     */
    template<typename T,
        typename=typename std::enable_if<std::is_pod<T>::value>::type
        >
    ref_t operator*(T x)const
    {
        PROTON_THROW_IF(*this==none,"want to * null values");
        refc_t* p=(refc_t*)alloc_t::duplicate(_rp);
        if(!p)
            throw std::bad_alloc();
        new (p) refc_t();
        obj_t* q=(obj_t *)(p+1);
        new (q) obj_t(__o()*x);
        return ref_t(alloc_inner,p,q);
    }

    /** ref_t % other
     */
    template<typename T>
    ref_t operator%(const T& x)const
    {
        PROTON_THROW_IF(*this==none, "want to % null values");
        refc_t* p=(refc_t*)alloc_t::duplicate(_rp);
        if(!p)
            throw std::bad_alloc();
        new (p) refc_t();
        obj_t* q=(obj_t *)(p+1);
        new (q) obj_t(__o() % x);
        return ref_t(alloc_inner,p,q);
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

template<typename refT>
para_<refT> para(const refT& t)
{
	return para_<refT>(t);
}


} // ns proton


#endif /* PROTON_PARA_HPP_ */
