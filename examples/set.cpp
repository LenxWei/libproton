#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/set.hpp>
#include <proton/string.hpp>

using namespace std;
using namespace proton;

int main()
{
    cout << ">>> set_ examples :" << endl;

    set_<int> s={0,1,2}, s1=s;
    set_<int> t={0,1,5};
    cout << "s is "<< s << endl;

    cout << "last item is :" << s[-1] << endl;
    PROTON_THROW_IF(s[2]!=s[-1], "[i] err");

    s << 3;
    PROTON_THROW_IF(!has(s,3), "has err")
    PROTON_THROW_IF(has(s,4), "has err")
    PROTON_THROW_IF(len(s)!=4, "insert err");

    s.remove(3);
    PROTON_THROW_IF(len(s)!=3, "del err");

    s=t|s;
    cout << "t|s=" << s << endl;
    PROTON_THROW_IF(len(s)!=4, "assign err");

    s=t&s;
    cout << "t&s=" << s << endl;
    PROTON_THROW_IF(s!=t, "assign err");

    s|=s1;
    cout << "t|s=" << s << endl;
    PROTON_THROW_IF(len(s)!=4, "assign err");

    s&=t;
    cout << "t&s=" << s << endl;
    PROTON_THROW_IF(s!=t, "assign err");

    auto v=s.pop();
    PROTON_THROW_IF(v!=0, "pop err");

    s >> v;
    cout << "pop : " << v << endl;

    return 0;
}

