#ifndef PROTON_MAP_HEADER
#define PROTON_MAP_HEADER

#include <map>
#include <iostream>

#include "_mapped_type.hpp"

namespace proton{

namespace detail{

template<typename _Key, typename _Tp, typename _Cmp, typename _Alloc>
struct has_t<std::map<_Key, _Tp, _Cmp, _Alloc> >{
    template<typename V> static bool result(const std::map<_Key, _Tp, _Cmp, _Alloc>& x, V&& v)
    {
        return x.find(v)!=x.end();
    }
};

} // ns detail

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

#endif // PROTON_MAP_HEADER
