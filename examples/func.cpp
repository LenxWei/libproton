#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/functor.hpp>
#include <string>

using namespace std;
using namespace proton;

typedef func_<int, const string&> _str2int;
typedef ref_<_str2int> str2int;

struct _f1: _str2int{
    int operator()(const string& s)
    {
        return s.size();
    }
};
typedef ref_<_f1> f1;

int main()
{
    str2int a;
    f1 b(alloc);
    a=b;
    cout << a("abc") << endl;
    return 0;
}

