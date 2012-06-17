#include <proton/base.hpp>
#include <proton/ref.hpp>
#include <proton/vector.hpp>
#include <proton/string.hpp>

using namespace std;
using namespace proton;

int main()
{
    cout << ">>> vector_ examples :" << endl;

    vector_<int> s={1,2,3};
    vector_<int> t={2,1};

    s.append(2);

    cout << "There is " << s.count(2) << " of two in s." << endl;
    PROTON_THROW_IF(s.count(2)!=2, "count err");

    s.extend(t);
    PROTON_THROW_IF(s.count(2)!=3, "count err");

    return 0;
}

