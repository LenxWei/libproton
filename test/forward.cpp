#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct base{};

struct deriv:base{};

void f(base&& x)
{
   cout << "f(base&&)" << endl;
}
void f(int&& x)
{
   cout << "f(int&&)" << endl;
}

void f(const base&& x)
{
   cout << "f(const base&&)" << endl;
}

void f(base& x)
{
   cout << "f(base&)" << endl;
}
void f(int& x)
{
   cout << "f(int&)" << endl;
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

   cout << "original:" << endl;
   f(g());
   f(h());
   f(a);
   f(b);

   cout << "forward:" << endl;
   f1(g());
   f1(h());
   f1(a);
   f1(b);

   cout << "for class:" << endl;
   deriv c[2];
   for(auto x:c){
      f(x);
   } 

   cout << "for int:" << endl;
   int d[2];
   for(auto x:d){
     f(x);
   }

   cout << "for int&&:" << endl;
   for(auto x:[](){return vector<int>({1});}()){
     f(x);
   }
   return 0;
}
