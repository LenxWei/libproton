#ifndef PROTON_UT_H
#define PROTON_UT_H

#include <vector>

namespace proton{
namespace detail{

/////////////////////////////
// unittest

//@return 0 - success
typedef int (*unittest_t)();

int unittest_run(std::vector<unittest_t>& ut);

}}
#endif // PROTON_UT_H
