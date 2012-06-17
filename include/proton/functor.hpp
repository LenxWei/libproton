#ifndef PROTON_FUNCTOR_H
#define PROTON_FUNCTOR_H

/** @file functor.hpp
 *  @brief functor interface
 */

#include <iostream>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <cstdio>

namespace proton{

/**
 * @addtogroup ref
 * @{
 * @addtogroup functor
 * @{
 */

template<typename> struct fo_;

/** functor interface for function object classes.
 */
template<typename retT, typename ...argT>
struct fo_<retT(argT...)>{
    virtual retT operator()(argT ...x)=0;
};


/** functor template.
 */
template<typename fpT>
    using func_=ref_<fo_<fpT> >;

/** @example func.cpp
 */

template<typename> struct _fp_;

template<typename retT, typename ...argT>
struct _fp_<retT(argT...) >: fo_<retT(argT...)>{
    typedef retT (*fp_t)(argT...);
    fp_t _f;

    _fp_()=delete;

    _fp_(fp_t f):_f(f)
    {}

    retT operator()(argT ...x)
    {
        return _f(x...);
    }
};

/** functor for normal functions.
 */
template<typename fpT>
    using fp_=ref_<_fp_<fpT> >;


template<typename, typename> struct _fm_;

template<typename refT, typename retT, typename ...argT>
struct _fm_<refT, retT(argT...) >: fo_<retT(argT...)>{
    typedef typename refT::obj_t obj_t;
    typedef retT (obj_t::*fp_t)(argT...);

    refT _x;
    fp_t _f;

    _fm_()=delete;

    _fm_(refT x, fp_t f):_x(x), _f(f)
    {}

    retT operator()(argT ...a)
    {
        return ((*_x).*(_f))(a...);
    }
};

/** functor for member functions.
 */
template<typename refT, typename fpT>
    using fm_=ref_<_fm_<refT, fpT> >;

/**
 * @}
 * @}
 */
}; // namespace proton

#endif // PROTON_FUNCTOR_H
