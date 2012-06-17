#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/functor.hpp>
#include <proton/string.hpp>

using namespace std;
using namespace proton;

typedef func_<unsigned int(const char*)> _str2int;
typedef ref_<_str2int> str2int;

typedef ref_<fp_<_str2int> > str2int_f1;

typedef ref_<fp_<unsigned int(const char*)> > str2int_f2;

struct _f1: _str2int{
    unsigned int operator()(const char* s)
    {
        return string(s).size();
    }
};
typedef ref_<_f1> str2int_fn;

struct _type{
    unsigned int f(const char* s)
    {
        return strlen(s);
    }
};

int main()
{
    cout << ">>> functor examples :" << endl;
    str2int a;
    str2int_fn b(alloc);
    a=b;
    cout << "functor is " << a("abc") << endl;
    PROTON_THROW_IF(a("abc")!=3, "err");

    str2int_f1 c(strlen);
    a=c;
    cout << "strlen fp_<func_ > is " << a("abc") << endl;
    PROTON_THROW_IF(a("abc")!=3, "err");

    str2int_f2 d(strlen);
    a=c;
    cout << "strlen fp_ directly is " << a("abc") << endl;
    PROTON_THROW_IF(a("abc")!=3, "err");

    return 0;
}

