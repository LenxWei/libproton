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
 * @addtogroup functor Functor
 * General functor interface template.
 * @{
 */

/** the common functor interface template for object classes.
 */
template<typename retT, typename ...argT>
struct func_{
    virtual retT operator()(argT&& ...x)=0;
};

/** @example func.cpp
 */

/**
 * @}
 * @}
 */
}; // namespace proton

#endif // PROTON_FUNCTOR_H
