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
    fp_t fp;

    _fp_()=delete;

    _fp_(fp_t f):fp(f)
    {}

    retT operator()(argT ...x)
    {
        return fp(x...);
    }
};

/** functor for normal functions.
 */
template<typename fpT>
    using fp_=ref_<_fp_<fpT> >;

/**
 * @}
 * @}
 */
}; // namespace proton

#endif // PROTON_FUNCTOR_H
