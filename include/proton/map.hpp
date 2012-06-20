#ifndef PROTON_MAP_HEADER
#define PROTON_MAP_HEADER

#include <map>
#include <iostream>
#include <stdexcept>

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

template <typename _Key, typename _Tp, typename _Cmp, typename _Alloc>
std::map<_Key, _Tp, _Cmp, _Alloc>& operator<<(std::map<_Key, _Tp, _Cmp, _Alloc>& x,
                                              const std::pair<const _Key, _Tp>& item)
{
    x.insert(item);
    return x;
}

template <typename _Key, typename _Tp, typename _Cmp, typename _Alloc, typename T>
std::map<_Key, _Tp, _Cmp, _Alloc>& operator<<(std::map<_Key, _Tp, _Cmp, _Alloc>& x,
                                              std::initializer_list<T> item)
{
    x.insert(item);
    return x;
}

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

template <typename _Key, typename _Tp, typename _Cmp, typename _Alloc>
std::wostream& operator<<(std::wostream& s, const std::map<_Key, _Tp, _Cmp, _Alloc>& x)
{
    s << L"{";
    bool first=true;
    for(auto& t: x){
        if(first)
            first=false;
        else
            s << L", ";
        s << t.first << L" : "<<t.second;
    }
    s << L"}";
    return s;
}

/** a map extension implementing python's dict-like interfaces.
 */
template <typename K, typename T, typename C=std::less<K>, typename A=smart_allocator<std::pair<K,T> > >
class map_ : public std::map<K,T,C,A>{
public:
    typedef std::map<K,T,C,A> baseT;
    typedef std::pair<K,T> itemT;

public:
    /** forwarding ctor.
     */
    template<typename ...argT> map_(argT&& ...a):baseT(a...)
    {}

    /** initializer_list forwarding ctor.
     */
    map_(std::initializer_list<T> a):baseT(a)
    {}

    /** copy ctor.
     */
    map_(const map_& x):baseT(x)
    {}

    /** move ctor.
     */
    map_(map_&& x)noexcept:baseT(x)
    {}

    explicit map_(const baseT& x):baseT(x)
    {}

    map_(baseT&& x)noexcept:baseT(x)
    {}

    /** assign.
     */
    map_& operator=(const map_& x)
    {
        baseT::operator=(x);
        return *this;
    }

    map_& operator=(map_&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    map_& operator=(const baseT& x)
    {
        baseT::operator=(x);
        return *this;
    }

    map_& operator=(baseT&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    template<typename argT> map_& operator=(argT&& a)
    {
        baseT::operator=(a);
        return *this;
    }

    map_& operator=(std::initializer_list<T> a)
    {
        baseT::operator=(a);
        return *this;
    }

    /** cast to std::set<>&.
     */
    operator baseT&()
    {
        return reinterpret_cast<baseT&>(*this);
    }

    PROTON_COPY_DECL(map_)

    /** remove an item.
     * @param key the key.
     * @throw std::out_of_range if there is no such a value.
     */
    void del(const K& key)
    {
        auto end=this->end();
        auto it=this->find(key);
        if(it==end)
            throw std::out_of_range("The given key doesn't exist in this map.");
        this->erase(it);
    }

    /** get an item from the map.
     * @param key the key of the item
     * @return the corresponding value.
     * @throw std::out_of_range if there is no such a key.
     */
    T get(const K& key)const
    {
        return this->at[key];
    }

    /** get an item from the map.
     * @param key the key of the item
     * @param dft the default value
     * @return the corresponding value, if the key doesn't exist, return the default.
     */
    template<typename ...D>
    T get(const K& key, D&& ...dft)const
    {
        auto it=this->find(key);
        if(it==this->end())
            return T(dft...);
        T r=it->second;
        this->erase(it);
        return r;
    }

    /** deprecated, using has()
     */
    bool has_key(const K& key)const
    {
        return this->find(key)!=this->end();
    }

    /** pop an item from the map.
     * @param key the key of the item
     * @return the corresponding value.
     * @throw proton::err if there is no such a key.
     */
    T pop(const K& key)
    {
        auto it=this->find(key);
        PROTON_THROW_IF(it==this->end(), "try to pop an non-existing key.");
        T r=it->second;
        this->erase(it);
        return r;
    }

    /** pop an item from the map.
     * @param key the key of the item
     * @param dft the default value
     * @return the corresponding value, if the key doesn't exist, return the default.
     */
    template<typename ...D>
    T pop(const K& key, D&& ...dft)
    {
        auto it=this->find(key);
        if(it==this->end())
            return T(dft...);
        T r=it->second;
        this->erase(it);
        return r;
    }

    /** pop the first item.
     * @throw proton::err if there is no items in the map.
     */
    itemT popitem()
    {
        PROTON_THROW_IF(this->empty(), "try to pop an item from an empty map");
        auto it=this->begin();
        itemT r=*it;
        this->erase(it);
        return r;
    }

    /** If key exists, return its value. If not, insert key with a value of default and return default.
     */
    template<typename ...D>
    const T& setdefault(const K& key, D&&... dft)
    {
        auto it=this->find(key);
        if(it!=this->end())
            return it->second;
        return this->insert(std::make_pair(key, T(dft...))).first->second;
    }

    template<typename oT>
    void update(oT&& o)
    {
        for(auto i:o){
            (*this)[i->first]=i->second;
        }
    }
};

/**
 * @example set.cpp
 */

/** cast to proton::map_<>& from std::map<>&.
 */
template<typename K, typename T, typename C, typename A>
map_<K,T,C,A>& cast_(std::map<K,T,C,A>& x)
{
    return reinterpret_cast<map_<K,T,C,A>&>(x);
}

template<typename K, typename T, typename C, typename A>
const map_<K,T,C,A>& cast_(const std::map<K,T,C,A>& x)
{
    return reinterpret_cast<const map_<K,T,C,A>&>(x);
}

template<typename K, typename T, typename C, typename A>
map_<K,T,C,A>&& cast_(std::map<K,T,C,A>&& x)
{
    return reinterpret_cast<map_<K,T,C,A>&&>(x);
}

template<typename K, typename T, typename C, typename A>
const map_<K,T,C,A>&& cast_(const std::map<K,T,C,A>&& x)
{
    return reinterpret_cast<const map_<K,T,C,A>&&>(x);
}

/**
 * @}
 */

}

#endif // PROTON_MAP_HEADER
