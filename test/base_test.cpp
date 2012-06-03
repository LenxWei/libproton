#include <iostream>
#include <vector>
#include "proton/base.hpp"
#include "proton/detail/unit_test.hpp"

int test_log()
{
    PROTON_LOG(1,"hello world!");
    return 0;
}

int test_assert()
{
    PROTON_THROW_IF(1, "hi world!");
    return 0;
}

int test_err()
{
    PROTON_ERR("we get the world~");
    return 0;
}

int main()
{
    proton::debug_level=1;
    proton::wait_on_err=0;
    std::vector<proton::detail::unittest_t> ut=
        {test_log, test_assert, test_err};
    proton::detail::unittest_run(ut);
    std::cout << "Don't worry. We are testing PROTON_THROW_IF & PROTON_ERR." << std::endl;
    return 0;
}

