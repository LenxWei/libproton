#ifndef PROTON_UNORDERED_MAP_HEADER
#define PROTON_UNORDERED_MAP_HEADER

#include <unordered_map>
#include <iostream>

#include "_mapped_type.hpp"

namespace proton{

template <typename K, typename V, typename H, typename E, typename A, typename T>
bool has(const std::unordered_map<K,V,H,E,A>& x, T&& key)
{
    return x.find(key)!=x.end();
}

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
    each(it, x){
        if(first)
            first=false;
        else
            s <<", ";
        s << it->first << " : "<<it->second;
    }
    s << "}";
    return s;
}

}

#endif // PROTON_UNORDERED_MAP_HEADER
