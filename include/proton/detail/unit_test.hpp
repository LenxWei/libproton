#ifndef PROTON_UT_H
#define PROTON_UT_H

/** @file unit_test.hpp
 *  @brief a lite unit test framework.
 */

#include <vector>

namespace proton{
namespace detail{

/////////////////////////////
// unittest

/** the prototype of unit test.
 * @return 0 - success
 */
typedef int (*unittest_t)();

/** unit test executor.
 * @param ut unit tests need to be tested.
 * @return 0: succes, other: failure
 */
int unittest_run(std::vector<unittest_t>& ut);

}}
#endif // PROTON_UT_H
