#include <iostream>

#include "proton/base.hpp"
#include "proton/ref.hpp"
#include "proton/map.hpp"
#include "proton/unordered_map.hpp"
#include "proton/vector.hpp"
#include "proton/string.hpp"
#include "proton/set.hpp"
#include "proton/unordered_set.hpp"
#include "proton/list.hpp"

using namespace std;
using namespace proton;

struct _tt{
protected:
    int _i;
public:
    _tt():_i(0)
    {

    }

    _tt(int i):_i(i)
    {

    }

    std::ostream& output(std::ostream& s)const
    {
        s << _i;
        return s;
    }

    typedef _tt keyed_self_t;
    int key()const
    {
        return _i;
    }

    void copy_to(void* p)const
    {
        new (p) _tt(*this);
    }
/*
    bool operator==(const _tt& t)
    {
        return _i==t._i;
    }
    bool operator<(const _tt& t)
    {
        return _i<t._i;
    }
*/
};

typedef ref_<_tt> tt;

struct _tt1{
protected:
    int _i;
public:
    _tt1():_i(0)
    {

    }

    _tt1(int i):_i(i)
    {

    }

    std::ostream& output(std::ostream& s)const
    {
        s << _i;
        return s;
    }

    bool operator==(const _tt1& t)const
    {
        return _i==t._i;
    }
    bool operator<(const _tt1& t)const
    {
        return _i<t._i;
    }

};

typedef ref_<_tt1> tt1;

struct _tt2{
protected:
    int _i;
public:
    _tt2():_i(0)
    {

    }

    _tt2(int i):_i(i)
    {

    }

    std::ostream& output(std::ostream& s)const
    {
        s << _i;
        return s;
    }

    typedef _tt2 keyed_self_t;
    tuple<int> key()const
    {
        return tuple<int>{_i};
    }
/*
    bool operator==(const _tt& t)
    {
        return _i==t._i;
    }
    bool operator<(const _tt& t)
    {
        return _i<t._i;
    }
*/
};

typedef ref_<_tt2> tt2;


int main(int argc, char** argv)
{
    wait_on_err=0;
    {
        list<int> a={1,2,3}, b={4,5,6};
        a.splice(a.end(), b);
        cout << a << endl;
    }
    {
        vector<string> a={"1","2","3","4"};

        PROTON_THROW_IF(has(a,"5"), "");
        PROTON_THROW_IF(!has(a,"1"),"");
        a << "5";
        PROTON_THROW_IF(!has(a,"5"), "");

        vector<string> b;
        for(auto& t: a){
            b << t;
        }
        PROTON_THROW_IF(a!=b, "");
    }
    {
        vector<int> a={1,2,3,4};
        PROTON_THROW_IF(has(a,5), "");
        PROTON_THROW_IF(!has(a,1),"");
        a << 5;
        PROTON_THROW_IF(!has(a,5), "");
    }
    {
        tt t(2), t1, t2(3);
        PROTON_THROW_IF(t>t2,"");
        PROTON_THROW_IF(t.__o()==t2.__o(),"");
        PROTON_THROW_IF(t==t2,"");
        cout << t << endl;

        t1=copy(t);
        PROTON_THROW_IF(t1!=t,"");
        cout << t1 << endl;
        ref_count(t1);

        PROTON_THROW_IF(is_null(t),"");
        PROTON_THROW_IF(!is_valid(t),"");
        //cout << is_null(t) <<","<< is_valid(t) << endl;
        reset(t);
        PROTON_THROW_IF(is_valid(t), "t is valid after resetting");
        PROTON_THROW_IF(!is_null(t), "t is not null after resetting");
    }
    {
        tt t1(34), t2(345), t0;
        unordered_set<tt, key_hash<tt> > s={t1,t2,t0};
        PROTON_THROW_IF(s.size()!=3,"");
    }
    {
        tt1 t1(34), t2(345), t0;
        set<tt1> s={t1,t2,t0};
        PROTON_THROW_IF(s.size()!=3,"");
    }
    {
        tt2 t1(34), t2(345), t0;
        unordered_set<tt2, subkey_hash<tt2> > s={t1,t2,t0};
        PROTON_THROW_IF(s.size()!=3,"");
    }
    {
        map<int,int> dict={{1,1},{2,2}};
        dict.insert({3,3});
        PROTON_THROW_IF(dict.size()!=3, "");
    }
    {
        map<int, string> dict={{1,"a"},{2,"b"}};
        string x;
        PROTON_THROW_IF(test_get(x, dict, 3),"");
        PROTON_THROW_IF(!test_get(x, dict, 2),"");
        PROTON_THROW_IF(x!="b","");
        PROTON_THROW_IF(test_insert(dict,1,"c"),"");
        PROTON_THROW_IF(!test_insert(dict,9,"c"),"");
        PROTON_THROW_IF(test_insert_or_get(dict,9,x),"");
        PROTON_THROW_IF(x!="c","");
        PROTON_THROW_IF(!test_insert_or_get(dict,8,x),"");
        PROTON_THROW_IF(dict[8]!="c","");
        bool hit;
        string& t=get_or_create(hit, dict, 8, "d");
        PROTON_THROW_IF(!hit,"");
        string& t1=get_or_create(hit, dict, 7, "d");
        PROTON_THROW_IF(hit,"");
        PROTON_THROW_IF(t1!="d","");
        t1="e";
        PROTON_THROW_IF(dict[7]!="e","");

        PROTON_THROW_IF(has(dict,0),"0 is in dict!");
        PROTON_THROW_IF(!has(dict,2),"2 is not in dict!");
        string s=get(dict,1);
        PROTON_THROW_IF(s!="a","");
        int a=1;
        PROTON_THROW_IF(!has(dict,a),"");
        string dft="x";
        //get_ref(dict,3,"x"); ///< this line should not pass by compilation
        auto s0=get(dict,3,dft);
        auto s1=get(dict,3,"x");
        PROTON_THROW_IF(s1!=s0,"adf");
        PROTON_THROW_IF(get(dict,4,"x")!=s0,"adf");
    }
    {
        unordered_map<int, string> dict={{1,"a"},{2,"b"}};
        string x;
        PROTON_THROW_IF(test_get(x, dict, 3),"");
        PROTON_THROW_IF(!test_get(x, dict, 2),"");
        PROTON_THROW_IF(x!="b","");
        PROTON_THROW_IF(test_insert(dict,1,"c"),"");
        PROTON_THROW_IF(!test_insert(dict,9,"c"),"");
        PROTON_THROW_IF(test_insert_or_get(dict,9,x),"");
        PROTON_THROW_IF(x!="c","");
        PROTON_THROW_IF(!test_insert_or_get(dict,8,x),"");
        PROTON_THROW_IF(dict[8]!="c","");
        bool hit;
        string& t=get_or_create(hit, dict, 8, "d");
        PROTON_THROW_IF(!hit,"");
        string& t1=get_or_create(hit, dict, 7, "d");
        PROTON_THROW_IF(hit,"");
        PROTON_THROW_IF(t1!="d","");
        t1="e";
        PROTON_THROW_IF(dict[7]!="e","");

        cout << "unordered_map:" << dict << endl;
        PROTON_THROW_IF(has(dict,0),"0 is in dict!");
        PROTON_THROW_IF(!has(dict,2),"2 is not in dict!");
        string s=get(dict,1);
        PROTON_THROW_IF(s!="a","");
        int a=1;
        PROTON_THROW_IF(!has(dict,a),"");
        string dft="x";
        //get_ref(dict,3,"x"); ///< this line should not pass by compilation
        auto s0=get(dict,3,dft);
        auto s1=get(dict,3,"x");
        PROTON_THROW_IF(s1!=s0,"adf");
        PROTON_THROW_IF(get(dict,4,"x")!=s0,"adf");
    }
    {
        map<string, int> dict={{"1",3},{"2",4}};
        cout << "map:" << dict << endl;
        PROTON_THROW_IF(has(dict,"0"),"0 is in dict!");
        PROTON_THROW_IF(!has(dict,"2"),"2 is not in dict!");
        int s=get(dict,"1");
        PROTON_THROW_IF(s!=3,"");
        int dft=0;
        //get_ref(dict,3,"x"); ///< this line should not pass by compilation
        auto s0=get(dict,"3",dft);
        auto s1=get(dict,"3",0);
        PROTON_THROW_IF(s1!=s0,"adf");
        PROTON_THROW_IF(get(dict,"4",0)!=s0,"adf");
    }
    {
        unordered_map<string, int> dict={{"1",3},{"2",4}};
        cout << "map:" << dict << endl;
        PROTON_THROW_IF(has(dict,"0"),"0 is in dict!");
        PROTON_THROW_IF(!has(dict,"2"),"2 is not in dict!");
        int s=get(dict,"1");
        PROTON_THROW_IF(s!=3,"");
        int dft=0;
        //get_ref(dict,3,"x"); ///< this line should not pass by compilation
        auto s0=get(dict,"3",dft);
        auto s1=get(dict,"3",0);
        PROTON_THROW_IF(s1!=s0,"adf");
        PROTON_THROW_IF(get(dict,"4",0)!=s0,"adf");
    }
    {
        set<int> a={1,2,3,4}, a1={1,2}, a2={3,4}, a3;
        set<string> s={"1","2","3","4"};
        PROTON_THROW_IF(has(a,5),"");
        PROTON_THROW_IF(!has(a,1),"");
        PROTON_THROW_IF(has(s,"5"),"");
        PROTON_THROW_IF(!has(s,"3"),"");

        a3=a&a2;
        auto a5=a&a1;
        a&=a1;
        PROTON_THROW_IF(a5!=a, "a5:"<<a5 << "!=a:"<<a);
        a3 << 1;
        PROTON_THROW_IF(a!=a1,"");
        auto a4=a3|a;
        a|=a3;
        PROTON_THROW_IF(a!=a4,"");
        cout << "set:" << a << endl;
        PROTON_THROW_IF(a.size()!=4,"");
        PROTON_THROW_IF(a2==a3,"");
    }
    {
        unordered_set<int> a={1,2,3,4}, a1={1,2}, a2={3,4}, a3;
        cout << "unordered_set:" << a << endl;
        unordered_set<string> s={"1","2","3","4"};
        PROTON_THROW_IF(has(a,5),"");
        PROTON_THROW_IF(!has(a,1),"");
        PROTON_THROW_IF(has(s,"5"),"");
        PROTON_THROW_IF(!has(s,"3"),"");

        a3=a&a2;
        auto a5=a&a1;
        a&=a1;
        PROTON_THROW_IF(a5!=a, "a5:"<<a5 << "!=a:"<<a);
        a3 << 1;
        PROTON_THROW_IF(a!=a1,"");
        auto a4=a3|a;
        a|=a3;
        PROTON_THROW_IF(a!=a4,"");
        cout << "set:" << a << endl;
        PROTON_THROW_IF(a.size()!=4,"");
        PROTON_THROW_IF(a2==a3,"");
    }
    {
        vector<int> a;
        a << 1 << 2<< 3;
        int x,y,z,d;
        a >> x >> y >> z;
        PROTON_THROW_IF(z!=1, "");
        bool got=false;
        try{
            a>>d;
        }
        catch(err& e){
            got=true;
        }
        PROTON_THROW_IF(!got, "");
    }
    {
        set<int> a;
        a << 1 << 2<< 3;
        int x,y,z,d;
        a >> x >> y >> z;
        PROTON_THROW_IF(z!=3, "");
        bool got=false;
        try{
            a>>d;
        }
        catch(err& e){
            got=true;
        }
        PROTON_THROW_IF(!got, "");
    }
    {
        vector<int> a={1,2,3,4,5};
        cout <<"vector:" << a << endl;
        PROTON_THROW_IF(get(a,-1)!=a[4], "");

    }
    {
        list<int> a={1,2};
        cout <<"list:" <<  a << endl;
        a << 3 << 4 << 5;
        int x,y,z;
        a >> x >> y >> z;
        PROTON_THROW_IF(z!=3,"");
        PROTON_THROW_IF(get(a,1)!=2,"");
        PROTON_THROW_IF(get(a,-1)!=2,"");
        bool got;
        got=false;
        try{
            get(a,2);
        }
        catch(err& e){
            got=true;
        }
        PROTON_THROW_IF(!got,"");
        got=false;
        try{
            get(a,3);
        }
        catch(err& e){
            got=true;
        }
        PROTON_THROW_IF(!got,"");
        got=false;
        try{
            get(a,-3);
        }
        catch(err& e){
            got=true;
        }
        PROTON_THROW_IF(!got,"");
        got=false;
        try{
            get(a,-4);
        }
        catch(err& e){
            got=true;
        }
        PROTON_THROW_IF(!got,"");
    }
    {
        string a="012345";
        PROTON_THROW_IF(sub(a,0,2)!="01","");
        PROTON_THROW_IF(sub(a,-3,-1)!="34","");
        PROTON_THROW_IF(get(a,-1)!='5',"");
        PROTON_THROW_IF(get(a,2)!='2',"");
    }
    {
        list<int> a={1,2,3};
        auto sub1=sub(a,1);
        auto sub2=sub(a,-2);
        PROTON_THROW_IF(sub1!=list<int>({2,3}),"");
        PROTON_THROW_IF(sub1!=sub2,"");
        PROTON_THROW_IF(sub(a,-3)!=a,"");
        PROTON_THROW_IF(sub(a,-4)!=a,"");
        PROTON_THROW_IF(sub(a,-5)!=a,"");
        PROTON_THROW_IF(!sub(a,3).empty(),"");
        PROTON_THROW_IF(!sub(a,4).empty(),"");
        PROTON_THROW_IF(!sub(a,5).empty(),"");
    }
    {
        list<int> a={1,2,3};
        PROTON_THROW_IF(!sub(a,1,1).empty(),"");
        PROTON_THROW_IF(!sub(a,2,1).empty(),"");
        PROTON_THROW_IF(!sub(a,-1,-1).empty(),"");
        PROTON_THROW_IF(!sub(a,-1,-2).empty(),"");

        PROTON_THROW_IF(!sub(a,3,4).empty(),"");
        PROTON_THROW_IF(!sub(a,5,3).empty(),"");

        PROTON_THROW_IF(sub(a,-5,5)!=a,"");
        PROTON_THROW_IF(sub(a,-4,-2)!=sub(a,0,1),"");
        PROTON_THROW_IF(sub(a,-3,-1)!=sub(a,0,2),"");
        PROTON_THROW_IF(sub(a,-4,-3)!=sub(a,1,1),"");
        PROTON_THROW_IF(sub(a,0,5)!=a,"");
        PROTON_THROW_IF(sub(a,0,3)!=a,"");
        PROTON_THROW_IF(!sub(a,-1,1).empty(),"");
        PROTON_THROW_IF(sub(a,-3,4)!=a,"");
        PROTON_THROW_IF(sub(a,-3,3)!=a,"");
        PROTON_THROW_IF(!sub(a,2,-2).empty(),"");
        PROTON_THROW_IF(!sub(a,2,-1).empty(),"");
        PROTON_THROW_IF(sub(a,1,-1).size()!=1,"");

    }
    {
        const vector<int> a={1,2,3,4,5};
        PROTON_THROW_IF(get(a,0)!=a[0], "");
        bool got=false;
        try{
            get(a,-6);
        }
        catch(err& e){
            got=true;
        }
        PROTON_THROW_IF(!got, "index checking error");
        auto a1=sub(a,1);
        auto a2=sub(a,1,-1);
        auto a3=sub(a1,0,-1);
        PROTON_THROW_IF(a2!=a3,"");
    }
    {
        string a="12345";
        PROTON_THROW_IF(get(a,0)!=a[0], "");
        bool got=false;
        try{
            get(a,-6);
        }
        catch(err& e){
            got=true;
        }
        PROTON_THROW_IF(!got, "index checking error");
        auto a1=sub(a,1);
        auto a2=sub(a,1,-1);
        auto a3=sub(a1,0,-1);
        PROTON_THROW_IF(a2!=a3,"");
        PROTON_THROW_IF(a2!="234", "");
    }
    cout << endl << ".-=<{ All unit tests passed. }>=-." << endl;
    return 0;
}
