#include <iostream>

using namespace std;

struct base{};

struct deriv:base{};

void f(base&& x)
{
   cout << "f(base&&)" << endl;
}

#if 0
void f(const base&& x)
{
   cout << "f(const base&&)" << endl;
}
#endif

void f(const base& x)
{
   cout << "f(const base&)" << endl;
}

deriv g()
{
   return deriv();
}

const deriv h()
{
   return deriv();
}

int main()
{
   f(g());
   f(h());
   return 0;
}
