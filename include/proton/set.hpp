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

/** @addtogroup set_
 * @{
 */

/** add an item in streaming style.
 * @param x the set to be added
 * @param val the new item
 * @return the new x
 */
template <typename T, typename C, typename A, typename V >
std::set<T,C,A>& operator<<(std::set<T,C,A>& x, V&& val)
{
    x.insert(val);
    return x;
}

/** pop an item in streaming style.
 * @param x the set to be popped from
 * @param val the popped item
 * @return the new x
 */
template <typename T, typename C, typename A, typename V >
std::set<T,C,A>& operator>>(std::set<T,C,A>& x, V& val)
{
    PROTON_THROW_IF(x.empty(), "want to pop an empty set.");
    val=*x.begin();
    x.erase(x.begin());
    return x;
}

/** general output for set.
 * @param s the output stream
 * @param x the set to be outputed
 * @return s
 */
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
std::wostream& operator<<(std::wostream& s, const std::set<T,C,A>& x)
{
    s << L"{";
    bool first=true;
    for(auto& t: x){
        if(first)
            first=false;
        else
            s <<L", ";
        s << t;
    }
    s << L"}";
    return s;
}

template <typename T, typename C, typename A>
void sort(std::set<T,C,A>& x)
{
}

/** a set extension implementing python's set-like interfaces.
 */
template<typename T, typename C=std::less<T>, typename A=smart_allocator<T> >
class set_ : public std::set<T,C,A>{
public:
    typedef std::set<T,C,A> baseT;

public:
    /** forwarding ctor.
     */
    template<typename ...argT> set_(argT&& ...a):baseT(a...)
    {}

    /** initializer_list forwarding ctor.
     */
    set_(std::initializer_list<T> a):baseT(a)
    {}

    /** copy ctor.
     */
    set_(const set_& x):baseT(x)
    {}

    /** move ctor.
     */
    set_(set_&& x)noexcept:baseT(x)
    {}

    explicit set_(const baseT& x):baseT(x)
    {}

    set_(baseT&& x)noexcept:baseT(x)
    {}

    /** assign.
     */
    set_& operator=(const set_& x)
    {
        baseT::operator=(x);
        return *this;
    }

    set_& operator=(set_&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    set_& operator=(const baseT& x)
    {
        baseT::operator=(x);
        return *this;
    }

    set_& operator=(baseT&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    template<typename argT> set_& operator=(argT&& a)
    {
        baseT::operator=(a);
        return *this;
    }

    set_& operator=(std::initializer_list<T> a)
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

    PROTON_COPY_DECL(set_)

    /** add an item.
     */
    void add(const T& x)
    {
        this->insert(x);
    }

    void add(T&& x)
    {
        this->insert(x);
    }

    /** pop an item from the sequence.
     * @return the value.
     * @throw proton::err if there is no any item.
     */
    T pop()
    {
        auto it=this->begin();
        PROTON_THROW_IF(it==this->end(), "try to pop an empty set.");
        T r=*it;
        this->erase(it);
        return r;
    }

    /** remove a value.
     * @param val the value.
     * @throw std::invalid_argument if there is no such a value.
     */
    void remove(const T& val)
    {
        auto end=this->end();
        auto it=this->find(val);
        if(it==end)
            throw std::invalid_argument("The given value doesn't exist in this sequence.");
        this->erase(it);
    }

    /** remove a value if it is present.
     * @param val the value.
     */
    void discard(const T& val)
    {
        auto end=this->end();
        auto it=this->find(val);
        if(it!=end)
            this->erase(it);
    }

};

/**
 * @example set.cpp
 */

/** &=
 */
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

/** |=
 */
template <typename T, typename C, typename A>
std::set<T,C,A>& operator|=(std::set<T,C,A>& x, const std::set<T,C,A>& y)
{
    for(auto& t: y){
        x.insert(t);
    }
    return x;
}

/** set & set
 */
template <typename T, typename C, typename A>
set_<T,C,A> operator&(const std::set<T,C,A>& x, const std::set<T,C,A>& y)
{
    set_<T,C,A> z;
    std::set_intersection(x.begin(), x.end(),y.begin(),y.end(),std::inserter(z,z.begin()));
    return z;
}


/** set | set
 */
template <typename T, typename C, typename A>
set_<T,C,A> operator|(const std::set<T,C,A>& x, const std::set<T,C,A>& y)
{
    set_<T,C,A> z;
    std::set_union(x.begin(), x.end(),y.begin(),y.end(),std::inserter(z,z.begin()));
    return z;
}

/** cast to proton::set_<>& from std::set<>&.
 */
template<typename T, typename C, typename A>
set_<T,C,A>& cast_(std::set<T,C,A>& x)
{
    return reinterpret_cast<set_<T,C,A>&>(x);
}

template<typename T, typename C, typename A>
const set_<T,C,A>& cast_(const std::set<T,C,A>& x)
{
    return reinterpret_cast<const set_<T,C,A>&>(x);
}

template<typename T, typename C, typename A>
set_<T,C,A>&& cast_(std::set<T,C,A>&& x)
{
    return reinterpret_cast<set_<T,C,A>&&>(x);
}

template<typename T, typename C, typename A>
const set_<T,C,A>&& cast_(const std::set<T,C,A>&& x)
{
    return reinterpret_cast<const set_<T,C,A>&&>(x);
}

/**
 * @}
 */

} // ns proton
#endif // PROTON_SET_HEADER
