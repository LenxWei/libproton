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
    PROTON_THROW_IF(x.empty(), "want to pop an empty set.");
    val=*x.begin();
    x.erase(x.begin());
    return x;
}

template <typename T, typename H, typename C, typename A>
std::ostream& operator<<(std::ostream& s, const std::unordered_set<T,H,C,A>& x)
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

template <typename T, typename H, typename C, typename A>
std::unordered_set<T,H,C,A> operator&(const std::unordered_set<T,H,C,A>& x, const std::unordered_set<T,H,C,A>& y)
{
    std::unordered_set<T,H,C,A> z;
    if(x.size()<y.size()){
        for(auto& t:x){
            if(has(y,t))
                z << t;
        }
        return z;
    }
    else{
        for(auto& t: y){
            if(has(x,t))
                z << t;
        }
        return z;
    }
}

template <typename T, typename H, typename C, typename A>
std::unordered_set<T,H,C,A> operator|(const std::unordered_set<T,H,C,A>& x, const std::unordered_set<T,H,C,A>& y)
{
    std::unordered_set<T,H,C,A> z;
    for(auto& t: x){
        z << t;
    }
    for(auto& t: y){
        z << t;
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
    for(auto& t: y){
        x << t;
    }
    return x;
}

}

#endif // PROTON_UNORDERED_SET_HEADER
