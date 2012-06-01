#ifndef PROTON_UNORDERED_SET_HEADER
#define PROTON_UNORDERED_SET_HEADER

#include <unordered_set>
#include <iterator>
#include <iostream>

namespace proton{

template <typename T, typename H, typename C, typename A, typename V >
bool has(const std::unordered_set<T,H,C,A>& x, V&& val)
{
    return x.find(val)!=x.end();
}

template <typename T, typename H, typename C, typename A, typename V >
std::unordered_set<T,H,C,A>& operator<<(std::unordered_set<T,H,C,A>& x, V&& val)
{
    x.insert(val);
    return x;
}

template <typename T, typename H, typename C, typename A, typename V >
std::unordered_set<T,H,C,A>& operator>>(std::unordered_set<T,H,C,A>& x, V& val)
{
    THROW_IF(x.empty(), "want to pop an empty set.");
    val=*x.begin();
    x.erase(x.begin());
    return x;
}

template <typename T, typename H, typename C, typename A>
std::ostream& operator<<(std::ostream& s, const std::unordered_set<T,H,C,A>& x)
{
    s << "{";
    bool first=true;
    each(it, x){
        if(first)
            first=false;
        else
            s <<", ";
        s << *it;
    }
    s << "}";
    return s;
}

template <typename T, typename H, typename C, typename A>
std::unordered_set<T,H,C,A> operator&(const std::unordered_set<T,H,C,A>& x, const std::unordered_set<T,H,C,A>& y)
{
    std::unordered_set<T,H,C,A> z;
    if(x.size()<y.size()){
        each(it,x){
            if(has(y,*it))
                z << *it;
        }
        return z;
    }
    else{
        each(it,y){
            if(has(x,*it))
                z << *it;
        }
        return z;
    }
}

template <typename T, typename H, typename C, typename A>
std::unordered_set<T,H,C,A> operator|(const std::unordered_set<T,H,C,A>& x, const std::unordered_set<T,H,C,A>& y)
{
    std::unordered_set<T,H,C,A> z;
    each(it, x){
        z << *it;
    }
    each(it, y){
        z << *it;
    }
    return z;
}

template <typename T, typename H, typename C, typename A>
std::unordered_set<T,H,C,A>& operator&=(std::unordered_set<T,H,C,A>& x,
                                        const std::unordered_set<T,H,C,A>& y)
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

template <typename T, typename H, typename C, typename A>
std::unordered_set<T,H,C,A>& operator|=(std::unordered_set<T,H,C,A>& x,
                                        const std::unordered_set<T,H,C,A>& y)
{
    each(it, y){
        x << *it;
    }
    return x;
}

}

#endif // PROTON_UNORDERED_SET_HEADER
