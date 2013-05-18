/*
 * weak.hpp
 *
 *  Created on: May 15, 2013
 *      Author: lenx
 */

#ifndef PROTON_WEAK_HPP
#define PROTON_WEAK_HPP

#include <proton/ref.hpp>

namespace proton{

/** weak_obj.
 * for weak referencing.
 */
class _weak_obj{
private:
	detail::refc_t* _rc;

public:
	_weak_obj(detail::refc_t* rc):_rc(rc)
	{}

	inline detail::refc_t* rc() const
	{
		return _rc;
	}

	inline void invalidate()
	{
		_rc=nullptr;
	}
};

typedef ref_<_weak_obj> weak_obj; // or refs_ ?

class weak_base{
private:
	weak_obj _wo;
public:
	weak_base():_wo()
	{}

	~weak_base()
	{
		if(is_valid(_wo))
			_wo->invalidate();
	}

private:
	weak_obj weak_ref(detail::refc_t* rc)
	{
		if(is_null(_wo)){
			_wo=weak_obj(rc);
		}
		return _wo;
	}
};

/** weak ptr.
 * T must be a ref_ type.
 * T::objT must be a subclass of weak_base.
 */
template<typename T>
class weak_{
public:
	typedef T::objT objT;
	static_assert(std::is_base_of<weak_base, objT>::value, "weak_ requires that T::objT must be a subclass of weak_base.");

private:
	weak_obj _wo;
	objT* _obj;

public:
	weak_(const T& r):_wo(r._rp), _obj(r._p)
	{
	}

	inline T lock()const
	{
		detail::refc_t* rc=_wo.rc();
		if(rc)
			return T(alloc_inner, rc, _obj);
		else
			return T(alloc_none);
	}
};

} // ns proton


#endif /* PROTON_WEAK_HPP */
