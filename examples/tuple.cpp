#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/tuple.hpp>
#include <proton/string.hpp>

using namespace std;
using namespace proton;

int main()
{
    cout << ">>> tuple examples :" << endl;

    tuple<str,str,str> s("a","b","c");
    tuple<str,str> t("b","a");

    cout << "s is " << s << endl;
    cout << "last item is :" << at<-1>(s) << endl;
    PROTON_THROW_IF(at<-1>(s)!="c", "get item err");

    auto slice=sub<1>(s); // s[1:]
    cout << "s[1:] is " << slice << endl;
    PROTON_THROW_IF(at<-1>(slice)!="c", "slice to the end err");

    auto slice1=sub<1,-1>(s); // s[1:-1]
    cout << "s[1:-1] is " << slice << endl;
    PROTON_THROW_IF(len(slice1)!=1, "slice in the middle err");

    auto slice2=slice1+s;
    cout << "s[1:-1]+s is " << slice2 << endl;
    PROTON_THROW_IF(len(slice2)!=4, "slice with step err");

/*
    cout << "min is :" << min(s) << endl;
    PROTON_THROW_IF(min(s)!="a", "min err");

    cout << "max is :" << max(s) << endl;
    PROTON_THROW_IF(max(s)!="c", "max err");
*/

    return 0;
}

