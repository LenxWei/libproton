#ifndef PROTON_SET_HEADER
#define PROTON_SET_HEADER

#include <set>
#include <iterator>
#include <iostream>

namespace proton{

namespace detail{

template<typename T, typename C, typename allocT>
struct has_t<std::set<T,C,allocT> >{
    template<typename V> static bool result(const std::set<T,C,allocT>& x, V&& v)
    {
        return x.find(v)!=x.end();
    }
};

} // ns detail

/*
template <typename T, typename C, typename allocT, typename V >
bool has<std::set<T,C,allocT>, V>(const std::set<T,C,allocT>& x, V&& val)
{
    return x.find(val)!=x.end();
}
*/

template <typename T, typename C, typename A, typename V >
std::set<T,C,A>& operator<<(std::set<T,C,A>& x, V&& val)
{
    x.insert(val);
    return x;
}

template <typename T, typename C, typename A, typename V >
std::set<T,C,A>& operator>>(std::set<T,C,A>& x, V& val)
{
    PROTON_THROW_IF(x.empty(), "want to pop an empty set.");
    val=*x.begin();
    x.erase(x.begin());
    return x;
}

template <typename T, typename C, typename A>
std::ostream& operator<<(std::ostream& s, const std::set<T,C,A>& x)
{
    s << "{";
    bool first=true;
    for(auto& t: x){
        if(first)
            first=false;
        else
            s <<", ";
        s << t;
    }
    s << "}";
    return s;
}

template <typename T, typename C, typename A>
std::set<T,C,A> operator&(const std::set<T,C,A>& x, const std::set<T,C,A>& y)
{
    std::set<T,C,A> z;
    std::set_intersection(x.begin(), x.end(),y.begin(),y.end(),std::inserter(z,z.begin()));
    return z;
}

template <typename T, typename C, typename A>
std::set<T,C,A>& operator&=(std::set<T,C,A>& x, const std::set<T,C,A>& y)
{
    auto it=x.begin(), end=x.end();
    while(it!=end){
        if(!has(y,*it)){
            auto it1=it;
            ++it;
            x.erase(it1);
        }
        else
            ++it;
    }
    return x;
}

template <typename T, typename C, typename A>
std::set<T,C,A> operator|(const std::set<T,C,A>& x, const std::set<T,C,A>& y)
{
    std::set<T,C,A> z;
    std::set_union(x.begin(), x.end(),y.begin(),y.end(),std::inserter(z,z.begin()));
    return z;
}

template <typename T, typename C, typename A>
std::set<T,C,A>& operator|=(std::set<T,C,A>& x, const std::set<T,C,A>& y)
{
    for(auto& t: y){
        x << t;
    }
    return x;
}

/*
template <typename T, typename A>
bool operator==(const std::vector<T,A>& x, const std::vector<T,A>& y)
{
    if(x.size()!=y.size())
        return false;
    auto it_y=y.begin();
    each(it_x,x){
        if(*it_x!=*it_y)
            return false;
        ++it_y;
    }
    return true;
}

template <typename T, typename A>
bool operator!=(const std::vector<T,A>& x, const std::vector<T,A>& y)
{
    if(x.size()!=y.size())
        return true;
    auto it_y=y.begin();
    each(it_x,x){
        if(*it_x!=*it_y)
            return true;
        ++it_y;
    }
    return false;
}
*/

template <typename T, typename C, typename A>
void sort(std::set<T,C,A>& x)
{
}

}

#endif // PROTON_SET_HEADER
