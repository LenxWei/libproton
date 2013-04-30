#ifndef PROTON_UNORDERED_SET_HEADER
#define PROTON_UNORDERED_SET_HEADER

#include <unordered_set>
#include <iterator>
#include <iostream>

#include <proton/base.hpp>
#include <proton/pool.hpp>
#include <proton/ref.hpp>

namespace proton{

namespace detail{

template<typename T, typename H, typename C, typename A>
struct has_t<std::unordered_set<T,H,C,A> >{
    template<typename V> static bool result(const std::unordered_set<T,H,C,A>& x, V&& v)
    {
        return x.find(v)!=x.end();
    }
};

} // ns detail

/** @addtogroup unordered_set_
 * @{
 */

/** add an item in streaming style.
 * @param x the unordered_set to be added
 * @param val the new item
 * @return the new x
 */
template <typename T, typename H, typename C, typename A, typename V >
std::unordered_set<T,H,C,A>& operator<<(std::unordered_set<T,H,C,A>& x, V&& val)
{
    x.insert(val);
    return x;
}

/** pop an item in streaming style.
 * @param x the unordered_set to be popped from
 * @param val the popped item
 * @return the new x
 */
template <typename T, typename H, typename C, typename A, typename V >
std::unordered_set<T,H,C,A>& operator>>(std::unordered_set<T,H,C,A>& x, V& val)
{
    PROTON_THROW_IF(x.empty(), "want to pop an empty unordered_set.");
    val=*x.begin();
    x.erase(x.begin());
    return x;
}

/** general output for unordered_set.
 * @param s the output stream
 * @param x the unordered_set to be outputed
 * @return s
 */
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

/** an unordered_set extension implementing python's unordered_set-like interfaces.
 */
template<typename T, typename H=std::hash<T>, typename E=std::equal_to<T>, typename A=smart_allocator<T> >
class unordered_set_ : public std::unordered_set<T,H,E,A>{
public:
    typedef std::unordered_set<T,H,E,A> baseT;

public:
    /** forwarding ctor.
     */
    template<typename ...argT> unordered_set_(argT&& ...a):baseT(a...)
    {}

    /** initializer_list forwarding ctor.
     */
    unordered_set_(std::initializer_list<T> a):baseT(a.begin(),a.end())
    {}

    /** copy ctor.
     */
    unordered_set_(const unordered_set_& x):baseT(x)
    {}

    /** move ctor.
     */
    unordered_set_(unordered_set_&& x)noexcept:baseT(x)
    {}

    explicit unordered_set_(const baseT& x):baseT(x)
    {}

    unordered_set_(baseT&& x)noexcept:baseT(x)
    {}

    /** assign.
     */
    unordered_set_& operator=(const unordered_set_& x)
    {
        baseT::operator=(x);
        return *this;
    }

    unordered_set_& operator=(unordered_set_&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    unordered_set_& operator=(const baseT& x)
    {
        baseT::operator=(x);
        return *this;
    }

    unordered_set_& operator=(baseT&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    template<typename argT> unordered_set_& operator=(argT&& a)
    {
        baseT::operator=(a);
        return *this;
    }

    unordered_set_& operator=(std::initializer_list<T> a)
    {
        baseT::operator=(a);
        return *this;
    }

    /** cast to std::unordered_set<>&.
     */
    operator baseT&()
    {
        return reinterpret_cast<baseT&>(*this);
    }

    PROTON_COPY_DECL_NV(unordered_set_)

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
        PROTON_THROW_IF(it==this->end(), "try to pop an empty unordered_set.");
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

    /** get an item based on its sequence in the unordered_set.
     *  @param i the index, if i<0, count from the tail (like python)
     *  @return the i-th item, or throw if i is out of range.
     */
    const T& operator[](long i)
    {
    	if(i<0){
    		i+=this->size();
    		if(i<0)
				throw std::out_of_range("The index is out of range.");
    	}

		auto it=this->begin();
		auto end=this->end();
		while(1){
			if(it==end){
				throw std::out_of_range("The index is out of range.");
			}
			if(i==0)
				return *it;
			i--;
			++it;
		}
    }
};

/**
 * @example unordered_set.cpp
 */

/** &
 */
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

/** |
 */
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

/** &=
 */
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

/** |=
 */
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
