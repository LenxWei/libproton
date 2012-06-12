#include <iostream>
#include <string>
#include "proton/base.hpp"
#include "proton/detail/unit_test.hpp"

int test_log()
{
    PROTON_LOG(1,"hello world!");
    return 0;
}

int test_assert()
{
    try{
        PROTON_THROW_IF(1, "hi world!");
    }
    catch(const proton::err& e){
        PROTON_THROW_IF(std::string("assert")!=e.what(), "bad exception!");
    }
    return 0;
}

int test_err()
{
    try{
        PROTON_ERR("we get the world~");
    }
    catch(const proton::err& e){
        PROTON_THROW_IF(std::string("err")!=e.what(), "bad exception!");
    }
    return 0;
}

int main()
{
    proton::debug_level=1;
    proton::wait_on_err=0;
    std::vector<proton::detail::unittest_t> ut=
        {test_log, test_assert, test_err};
    return proton::detail::unittest_run(ut);
}

