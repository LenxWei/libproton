#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/map.hpp>
#include <proton/string.hpp>

using namespace std;
using namespace proton;

int main()
{
    cout << ">>> map_ examples :" << endl;

    map_<int,string> s={{0,"a"},{1,"b"},{2,"c"}};
    map_<int,string> t={{0,"b"},{1,"a"}};
    cout << "s is "<< s << endl;

    cout << "last item is :" << s[2] << endl;
    PROTON_THROW_IF(s[2]!="c", "get item err");

    s.update(t);
    PROTON_THROW_IF(s[0]!="b", "slice + s err");

    s.insert({3,"d"});
    auto v=s.get(3);
    PROTON_THROW_IF(v!="d", "insert err");
    PROTON_THROW_IF(!has(s,3), "has err")
    PROTON_THROW_IF(has(s,4), "has err")
    PROTON_THROW_IF(s.get(4,"e")!="e", "get err");
    PROTON_THROW_IF(len(s)!=4, "insert err");

    s.del(3);
    PROTON_THROW_IF(len(s)!=3, "del err");

    s=t;
    PROTON_THROW_IF(len(s)!=2, "assign err");

    v=s.pop(1);
    PROTON_THROW_IF(v!="a", "pop err");

    v=s.pop(1,"b");
    PROTON_THROW_IF(v!="b", "pop err");

    auto k=s.popitem();
    PROTON_THROW_IF(k.first!=0, "popitem err");

    return 0;
}

