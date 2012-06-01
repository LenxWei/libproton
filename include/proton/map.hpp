#ifndef PROTON_MAP_HEADER
#define PROTON_MAP_HEADER

#include <map>
#include <iostream>

#include "_mapped_type.hpp"

namespace proton{

template <typename _Key, typename _Tp, typename _Cmp, typename _Alloc, typename T>
bool has(const std::map<_Key, _Tp, _Cmp, _Alloc>& x, T&& key)
{
    return x.find(key)!=x.end();
}

/*
template <typename _Key, typename _Tp, typename _Cmp, typename _Alloc>
std::map<_Key, _Tp, _Cmp, _Alloc>& operator<<(std::map<_Key, _Tp, _Cmp, _Alloc>& x,
                                              const std::pair<const _Key, _Tp>& item)
{
    x.insert(item);
    return x;
}
*/

template <typename _Key, typename _Tp, typename _Cmp, typename _Alloc>
std::ostream& operator<<(std::ostream& s, const std::map<_Key, _Tp, _Cmp, _Alloc>& x)
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

#endif // PROTON_MAP_HEADER
