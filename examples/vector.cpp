#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/vector.hpp>
#include <proton/string.hpp>

using namespace std;
using namespace proton;

int main()
{
    cout << ">>> vector_ examples :" << endl;

    vector_<string> s={"a","b","c"};
    vector_<string> t={"b","a"};

    cout << "last item is :" << s[-1] << endl;
    PROTON_THROW_IF(s[-1]!="c", "get item err");

    vector_<string> slice=s(1); // s[1:]
    PROTON_THROW_IF(slice[-1]!="c", "slice to the end err");

    slice=s(1,-1); // s[1:-1]
    PROTON_THROW_IF(len(slice)!=1, "slice in the middle err");

    slice=s(0,3,2); // s[0:3:2]
    PROTON_THROW_IF(len(slice)!=2, "slice with step err");

    slice=slice*2;
    PROTON_THROW_IF(len(slice)!=4, "slice with step err");

    slice=2*slice;
    PROTON_THROW_IF(len(slice)!=8, "slice with step err");

    slice=slice+s;
    PROTON_THROW_IF(len(slice)!=11, "slice with step err");

    s.append("b");

    cout << "There is " << s.count("b") << " of b in s." << endl;
    PROTON_THROW_IF(s.count("b")!=2, "count err");

    s.extend(t);
    PROTON_THROW_IF(s.count("b")!=3, "extend err");

    int i=s.index("c");
    PROTON_THROW_IF(i!=2, "index err");

    s.insert(-1,"x");
    PROTON_THROW_IF(s.index("x")!=long(s.size()-2), "index err");

    string k=s.pop();
    k=s.pop();
    PROTON_THROW_IF(k!="x", "pop err");

    size_t n=s.count("b");
    s.remove("b");
    PROTON_THROW_IF(s.count("b")!=n-1, "remove err");

    string r=s[-2];
    s.reverse();
    cout << "reversed vector is :" << s << endl;
    PROTON_THROW_IF(s[1]!=r, "remove err");

    s.sort();
    cout << "sorted vector is :" << s << endl;
    PROTON_THROW_IF(s[0]!="a", "remove err");
    return 0;
}

