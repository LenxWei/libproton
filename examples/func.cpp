#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/functor.hpp>
#include <proton/string.hpp>

using namespace std;
using namespace proton;

typedef func_<unsigned int, const char*> _str2int;
typedef ref_<_str2int> str2int;

struct _f1: _str2int{
    unsigned int operator()(const char* s)
    {
        return string(s).size();
    }
};
typedef ref_<_f1> f1;
typedef ref_<fp_<unsigned int, const char*> > f2;

int main()
{
    str2int a;
    f1 b(alloc);
    f2 c(strlen);
    a=b;
    cout << a("abc") << endl;
    a=c;
    cout << a("abc") << endl;
    return 0;
}

