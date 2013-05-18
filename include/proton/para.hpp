/*
 * para.hpp
 *
 *  Created on: May 18, 2013
 *      Author: lenx
 */

#ifndef PROTON_PARA_HPP_
#define PROTON_PARA_HPP_

#include <proton/base.hpp>
#include <proton/ref.hpp>

namespace proton{


template<typename T>
class para_ {
private:
	constexpr static long id_shift=sizeof(long)/2;
	constexpr static long offset_mask =long((unsigned long)(-1)>>id_shift);

	long _id_and_offset; // lowest half is offset, highest half is id
	T* _p;

	long compute_id_offset(long id, long offset)
	{
		// [ATTENTION!] offset should be less than 65536 on 32bit!
		return (id<<id_shift)+(offset & offset_mask);
	}

	para_(const long* id, T* p)
		:_id_and_offset(compute_id_offset(*id,(char*)p-(char*)id)),
		 _p(p)
	{}

public:

	template<A,Tr>
	para_(const ref_<T,A,Tr>& r):para_(r.__rp()->id(), r.__p())
	{}

	bool validate()
	{
		long offset=_id_and_offset & offset_mask;
		long* id=(long*)((char*)p-offset);
		return ((*id) & offset_mask) == (_id_and_offset >> id_shift);
	}

	~para_()
	{
		PROTON_THROW_IF(!validate(), "para_ validation failed. please use ref_ instead.")
	}

    /** operator-> points to the object refered.
     */
    T* operator->()
    {
        return _p;
    }

    /** operator-> points to the object refered.
     */
    const T* operator->()const
    {
        return _p;
    }
};

} // ns proton


#endif /* PROTON_PARA_HPP_ */
