#include <iostream>
#include <vector>
#include "proton/base.hpp"
#include "proton/detail/unit_test.hpp"

int test_log()
{
    PROTON_LOG(1,"hello world!");
    return 0;
}

int main()
{
    proton::debug_level=1;
    std::vector<proton::detail::unittest_t> ut={test_log};
    proton::detail::unittest_run(ut);
    return 0;
}

