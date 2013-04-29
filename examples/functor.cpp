#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/functor.hpp>
#include <proton/string.hpp>
#include <proton/deque.hpp>

using namespace std;
using namespace proton;

/** define a function interface type.
 */
typedef long unsigned int (_str2int)(const char*);

/** define a functor type.
 */
typedef func_<_str2int> str2int;

/** You can use function interface types to define fp_ types.
 */
typedef fp_<_str2int> str2int_fp;

/** an function object type inheriting the interface.
 */
struct _f1: fo_<_str2int>{
    long unsigned int operator()(const char* s)
    {
        return string(s).size();
    }
};
typedef ref_<_f1> str2int_fo;

/** an object type with a candidate member function.
 */
struct _t1{
    /** the candidate member function.
     */
    long unsigned int f(const char* s)
    {
        return strlen(s);
    }
};
typedef ref_<_t1> t1;

typedef fm_<t1, _str2int> str2int_fm;

int main()
{
    cout << ">>> functor examples :" << endl;

    /// the interface variable.
    str2int a;

    /// generate a functor using the function object type.
    str2int_fo b(alloc);
    a=b;
    cout << "fo is " << a("abc") << endl;
    PROTON_THROW_IF(a("abc")!=3, "err");

    /// generate a functor using a library function.
    str2int_fp c(strlen);
    a=c;
    cout << "strlen through fp is " << a("abc") << endl;
    PROTON_THROW_IF(a("abc")!=3, "err");

    /// generate a functor using a lambda function.
    str2int_fp d([](const char* s){return strlen(s);});
    a=d;
    cout << "lambda through fp is " << a("abc") << endl;
    PROTON_THROW_IF(a("abc")!=3, "err");

    /// generate a functor using a ref_ class member function.
    t1 x;
    str2int_fm e(x, &_t1::f);
    a=e;
    cout << "member function through fm is " << a("abc") << endl;
    PROTON_THROW_IF(a("abc")!=3, "err");

    vector<char> buf={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
    cout << str(" ").join(List_map([](char x){return "%x"%_t(x);}, buf)) << endl;
    return 0;
}

