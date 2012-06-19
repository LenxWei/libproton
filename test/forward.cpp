#include <iostream>

using namespace std;

struct base{};

struct deriv:base{};

void f(base&& x)
{
   cout << "f(base&&)" << endl;
}

void f(const base&& x)
{
   cout << "f(const base&&)" << endl;
}

void f(base& x)
{
   cout << "f(base&)" << endl;
}

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

template<typename T>void f1(T&& x)
{
   f((T&&)x);
}

int main()
{
   deriv a;
   const deriv b;
   f(g());
   f(h());
   f(a);
   f(b);
   f1(g());
   f1(h());
   f1(a);
   f1(b);
   return 0;
}
