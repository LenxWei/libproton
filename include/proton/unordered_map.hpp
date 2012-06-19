#ifndef PROTON_UNORDERED_MAP_HEADER
#define PROTON_UNORDERED_MAP_HEADER

#include <unordered_map>
#include <iostream>

#include "_mapped_type.hpp"

namespace proton{

namespace detail{

template<typename K, typename V, typename H, typename E, typename A>
struct has_t<std::unordered_map<K,V,H,E,A> >{
    template<typename T> static bool result(const std::unordered_map<K,V,H,E,A>& x, T&& v)
    {
        return x.find(v)!=x.end();
    }
};

} // ns detail

/*
template <typename K, typename V, typename H, typename E, typename A, typename T>
std::unordered_map<K,V,H,E,A>& operator<<(std::unordered_map<K,V,H,E,A>& x,
                                              const std::pair<const K, V>& item)
{
    x.insert(item);
    return x;
}
*/

template <typename K, typename V, typename H, typename E, typename A>
std::ostream& operator<<(std::ostream& s, const std::unordered_map<K,V,H,E,A>& x)
{
    s << "{";
    bool first=true;
    for(auto& t: x){
        if(first)
            first=false;
        else
            s <<", ";
        s << t.first << " : "<<t.second;
    }
    s << "}";
    return s;
}

}

#endif // PROTON_UNORDERED_MAP_HEADER
