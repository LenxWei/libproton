#ifndef PROTON_UNORDERED_MAP_HEADER
#define PROTON_UNORDERED_MAP_HEADER

#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <proton/base.hpp>
#include <proton/pool.hpp>
#include <proton/ref.hpp>

#include "_mapped_type.hpp"

namespace proton{

/** @addtogroup unordered_map_
 * @{
 */

/** add an item in streaming style.
 * @param x the unordered_map to be added
 * @param item the new item
 * @return the new x
 */
template <typename _Key, typename _Tp, typename _Hash, typename _Eq, typename _Alloc>
std::unordered_map<_Key, _Tp, _Hash, _Eq, _Alloc>& operator<<(std::unordered_map<_Key, _Tp, _Hash, _Eq, _Alloc>& x,
                                              const std::pair<const _Key, _Tp>& item)
{
    x.insert(item);
    return x;
}

/** general output for unordered_map.
 * @param s the output stream
 * @param x the unordered_map to be outputed
 * @return s
 */
template <typename _Key, typename _Tp, typename _Hash, typename _Eq, typename _Alloc>
std::ostream& operator<<(std::ostream& s, const std::unordered_map<_Key, _Tp, _Hash, _Eq, _Alloc>& x)
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

template <typename _Key, typename _Tp, typename _Hash, typename _Eq, typename _Alloc>
std::wostream& operator<<(std::wostream& s, const std::unordered_map<_Key, _Tp, _Hash, _Eq, _Alloc>& x)
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

/** an unordered_map extension implementing python's dict-like interfaces.
 */
template <typename K, typename T, typename H=std::hash<K>, typename E=std::equal_to<K>,
		typename A=smart_allocator<std::pair<const K,T> > >
class unordered_map_ : public std::unordered_map<K,T,H,E,A>{
public:
    typedef std::unordered_map<K,T,H,E,A> baseT;
    typedef std::pair<const K,T> itemT;

public:
    /** forwarding ctor.
     */
    template<typename ...argT> unordered_map_(argT&& ...a):baseT(a...)
    {}

    /** initializer_list forwarding ctor.
     */
    unordered_map_(std::initializer_list<itemT> a):baseT(a)
    {}

    /** copy ctor.
     */
    unordered_map_(const unordered_map_& x):baseT(x)
    {}

    /** move ctor.
     */
    unordered_map_(unordered_map_&& x)noexcept:baseT(x)
    {}

    explicit unordered_map_(const baseT& x):baseT(x)
    {}

    unordered_map_(baseT&& x)noexcept:baseT(x)
    {}

    /** assign.
     */
    unordered_map_& operator=(const unordered_map_& x)
    {
        baseT::operator=(x);
        return *this;
    }

    unordered_map_& operator=(unordered_map_&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    unordered_map_& operator=(const baseT& x)
    {
        baseT::operator=(x);
        return *this;
    }

    unordered_map_& operator=(baseT&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    template<typename argT> unordered_map_& operator=(argT&& a)
    {
        baseT::operator=(a);
        return *this;
    }

    unordered_map_& operator=(std::initializer_list<T> a)
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

    PROTON_COPY_DECL_NV(unordered_map_)

    /** remove an item.
     * @param key the key.
     * @throw std::out_of_range if there is no such a value.
     */
    void del(const K& key)
    {
        auto end=this->end();
        auto it=this->find(key);
        if(it==end)
            throw std::out_of_range("The given key doesn't exist in this unordered_map.");
        this->erase(it);
    }

    /** get an item from the unordered_map.
     * @param key the key of the item
     * @return the corresponding value.
     * @throw std::out_of_range if there is no such a key.
     */
    T get(const K& key)const
    {
        return this->at(key);
    }

    /** get an item from the unordered_map.
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
        return r;
    }

    /** deprecated, using has()
     */
    bool has_key(const K& key)const
    {
        return this->find(key)!=this->end();
    }

    /** pop an item from the unordered_map.
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

    /** pop an item from the unordered_map.
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
     * @throw proton::err if there is no items in the unordered_map.
     */
    itemT popitem()
    {
        PROTON_THROW_IF(this->empty(), "try to pop an item from an empty unordered_map");
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

    /** Update the unordered_map with the key/value pairs from other, overwriting existing keys.
     */
    template<typename oT>
    void update(oT&& o)
    {
        for(auto i:o){
            (*this)[i.first]=i.second;
        }
    }
};

/**
 * @example unordered_map.cpp
 */

/** cast to proton::unordered_map_<>& from std::unordered_map<>&.
 */
template<typename K, typename T, typename H, typename E, typename A>
unordered_map_<K,T,H,E,A>& cast_(std::unordered_map<K,T,H,E,A>& x)
{
    return reinterpret_cast<unordered_map_<K,T,H,E,A>&>(x);
}

template<typename K, typename T, typename H, typename E, typename A>
const unordered_map_<K,T,H,E,A>& cast_(const std::unordered_map<K,T,H,E,A>& x)
{
    return reinterpret_cast<const unordered_map_<K,T,H,E,A>&>(x);
}

template<typename K, typename T, typename H, typename E, typename A>
unordered_map_<K,T,H,E,A>&& cast_(std::unordered_map<K,T,H,E,A>&& x)
{
    return reinterpret_cast<unordered_map_<K,T,H,E,A>&&>(x);
}

template<typename K, typename T, typename H, typename E, typename A>
const unordered_map_<K,T,H,E,A>&& cast_(const std::unordered_map<K,T,H,E,A>&& x)
{
    return reinterpret_cast<const unordered_map_<K,T,H,E,A>&&>(x);
}

/**
 * @}
 */

namespace detail{

template<typename _Key, typename _Tp, typename _Hash, typename _Eq, typename _Alloc>
struct has_t<std::unordered_map<_Key, _Tp, _Hash, _Eq, _Alloc> >{
    template<typename V> static bool result(const std::unordered_map<_Key, _Tp, _Hash, _Eq, _Alloc>& x, V&& v)
    {
        return x.find(v)!=x.end();
    }
};

template<typename _Key, typename _Tp, typename _Hash, typename _Eq, typename _Alloc>
struct has_t<unordered_map_<_Key, _Tp, _Hash, _Eq, _Alloc> >{
    template<typename V> static bool result(const unordered_map_<_Key, _Tp, _Hash, _Eq, _Alloc>& x, V&& v)
    {
        return x.find(v)!=x.end();
    }
};

} // ns detail

} // ns proton

#endif // PROTON_UNORDERED_MAP_HEADER
