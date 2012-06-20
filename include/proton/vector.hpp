#ifndef PROTON_VECTOR_HEADER
#define PROTON_VECTOR_HEADER


/** @file vector.hpp
 *  @brief vector support.
 *  Please include this header instead of \<vector\>.
 */

#include <vector>
#include <algorithm>
#include <iostream>
#include <initializer_list>
#include <algorithm>
#include <proton/pool.hpp>
#include <stdexcept>

namespace proton{

/** @addtogroup vector_
 * @{
 */

/** add an item in streaming style.
 * @param x the vector to be added
 * @param val the new item
 * @return the new x
 */
template <typename T, typename A, typename V>
std::vector<T,A>& operator<<(std::vector<T,A>& x, V&& val)
{
    x.push_back(val);
    return x;
}

/** add an item in streaming style.
 * @param x the vector to be added
 * @param val the new item
 * @return the new x
 */
template <typename T, typename A, typename V>
std::vector<T,A>& operator<<(std::vector<T,A>& x, const V& val)
{
    x.push_back(val);
    return x;
}

/** pop an item in streaming style.
 * @param x the vector to be popped from
 * @param val the popped item
 * @return the new x
 */
template <typename T, typename A, typename V>
std::vector<T,A>& operator>>(std::vector<T,A>& x, V& val)
{
    PROTON_THROW_IF(x.empty(), "want to pop an empty vector.");
    val=x.back();
    x.pop_back();
    return x;
}

/** general output for vector.
 * @param s the output stream
 * @param x the vector to be outputed
 * @return s
 */
template <typename T, typename A>
std::ostream& operator<<(std::ostream& s, std::vector<T,A>& x)
{
    s << "[";
    bool first=true;
    for(auto& t: x){
        if(first)
            first=false;
        else
            s <<", ";
        s << t;
    }
    s << "]";
    return s;
}

template <typename T, typename A>
std::wostream& operator<<(std::wostream& s, std::vector<T,A>& x)
{
    s << L"[";
    bool first=true;
    for(auto& t: x){
        if(first)
            first=false;
        else
            s <<L", ";
        s << t;
    }
    s << L"]";
    return s;
}

/* sort a vector
 * @param x the vector to be sorted
 */
template <typename T, typename A>
void sort(std::vector<T,A>& x)
{
    std::sort(x.begin(), x.end());
}

/* get an item like python.
 * @param x the sequence
 * @param i the index
 * @return x[i]
 */
template <typename T, typename A>
T& get(std::vector<T,A>& x, long i)
{
    unsigned long s=x.size();
    if(i<0)
        i=s+i;
    if(i<0 || (unsigned long)i >= s )
        PROTON_ERR("out of range: look up "<<i<<" in vector whose size is " << s);
    return x[i];
}

/* get a const item like python.
 * @param x the sequence
 * @param i the index
 * @return x[i]
 */
template <typename T, typename A>
const T& get(const std::vector<T,A>& x, long i)
{
    unsigned long s=x.size();
    if(i<0)
        i=s+i;
    if(i<0 || (unsigned long)i >= s )
        PROTON_ERR("out of range: look up "<<i<<" in vector whose size is " << s);
    return x[i];
}

/* get a slice like python.
 * @param x the sequence
 * @param first the start
 * @return x[first:]
 */
template <typename T, typename A>
std::vector<T,A> sub(const std::vector<T,A>& x, long first)
{
    std::vector<T,A> r;
    unsigned long s=x.size();
    if(first<0)
        first=first+s;
    if(first<0)
        first=0;
    if((unsigned long)first >= s )
        return r;

    auto start=x.begin();
    std::copy(start+first, x.end(), std::back_inserter(r));
    return r;
}

/* get a slice like python.
 * @param x the sequence
 * @param first the start
 * @param last  the end
 * @return x[first:last]
 */
template <typename T, typename A>
std::vector<T,A> sub(const std::vector<T,A>& x, long first, long last)
{
    std::vector<T,A> r;
    unsigned long s=x.size();
    if(first<0)
        first=first+s;
    if(first<0)
        first=0;
    if((unsigned long)first >= s )
        return r;

    if(last<0)
        last=last+s;
    if(last<=0)
        return r;

    if((unsigned long)last>s)
        last=s;
    auto start=x.begin();
    std::copy(start+first, start+last, std::back_inserter(r));
    return r;
}

/** a vector extension implementing python's list-like interfaces.
 */
template <typename T, typename A=smart_allocator<T> >
class vector_ : public std::vector<T,A>{
    public:
    typedef std::vector<T,A> baseT;
    typedef typename baseT::difference_type offset_t;

protected:
    offset_t __offset(offset_t i)const
    {
        if(i<0)
            i+=this->size();
        return i;
    }

    offset_t offset(offset_t i)const
    {
        i=__offset(i);
        PROTON_THROW_IF(i<0 || (size_t)i>=this->size(), "out of range, offset is " << i
                         << " while size is " << this->size() );
        return i;
    }

    void fix_range(offset_t& begin, offset_t& end)const
    {
        offset_t size=(offset_t)this->size();
        begin=__offset(begin);
        end=__offset(end);
        if(begin>=size || end<=0 || end<=begin){
            begin=0;
            end=0;
            return;
        }
        if(begin<0)
            begin=0;
        if(end>=size)
            end=size;
    }

public:
    /** forwarding ctor.
     */
    template<typename ...argT> vector_(argT&& ...a):baseT(a...)
    {}

    /** initializer_list forwarding ctor.
     */
    vector_(std::initializer_list<T> a):baseT(a)
    {}

    /** copy ctor.
     */
    vector_(const vector_& x):baseT(x)
    {}

    /** move ctor.
     */
    vector_(vector_&& x)noexcept:baseT(x)
    {}

    explicit vector_(const baseT& x):baseT(x)
    {}

    vector_(baseT&& x)noexcept:baseT(x)
    {}

    /** assign.
     */
    vector_& operator=(const vector_& x)
    {
        baseT::operator=(x);
        return *this;
    }

    vector_& operator=(vector_&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    vector_& operator=(const baseT& x)
    {
        baseT::operator=(x);
        return *this;
    }

    vector_& operator=(baseT&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    template<typename ...argT> vector_& operator=(argT&& ...a)
    {
        baseT::operator=(a...);
        return *this;
    }

    vector_& operator=(std::initializer_list<T> a)
    {
        baseT::operator=(a);
        return *this;
    }

    /** cast to std::vector<>&.
     */
    operator baseT&()
    {
        return reinterpret_cast<baseT&>(*this);
    }

    /** [i] in python
     */
    T& operator[](offset_t i)
    {
        return *(this->begin()+offset(i));
    }

    /** [i] in python
     */
    const T& operator[](offset_t i)const
    {
        return *(this->begin()+offset(i));
    }

    /** slice of [i:]
     */
    vector_ operator()(offset_t i)const
    {
        auto begin=this->begin();
        return vector_(begin+offset(i),this->end());
    }

    /** slice of [i:j]
     */
    vector_ operator()(offset_t i, offset_t j)const
    {
        auto begin=this->begin();
        fix_range(i,j);
        return vector_(begin+i,begin+j);
    }

    /** slice of [i:j:k]
     */
    vector_ operator()(offset_t i, offset_t j, size_t k)const
    {
        fix_range(i,j);
        vector_ r;
        r.reserve((j-i)/k+1);
        auto it=this->begin()+i;
        for(offset_t n=i; n<j; n+=k,it+=k)
            r.push_back(*it);
        return r;
    }

    /** append an item at the end.
     */
    void append(const T& x)
    {
        this->push_back(x);
    }

    void append(T&& x)
    {
        this->push_back(x);
    }

    /** total number of occurences of x.
     */
    size_t count(const T& x)const
    {
        return std::count(this->begin(), this->end(), x);
    }

    /** delete the i-th item
     */
    void del(offset_t i)
    {
        this->erase(this->begin()+offset(i));
    }

    /** delete from i-th to the j-th items
     */
    void del(offset_t i, offset_t j)
    {
        fix_range(i,j);
        auto begin=this->begin();
        this->erase(begin+i, begin+j);
    }

    /** delete from i-th item to the end
     */
    void del_to_end(offset_t i)
    {
        this->erase(this->begin()+offset(i), this->end());
    }

    /** append items from a sequence.
     */
    template<typename seqT>void extend(const seqT& x)
    {
        for(auto& i:x){
            this->push_back(i);
        }
    }

    template<typename seqT>void extend(seqT&& x)
    {
        for(auto&& i:x){
            this->push_back(i);
        }
    }

    /** index of the first occurence of a value.
     * @param val the value.
     * @throw std::invalid_argument if there is no such a value.
     */
    offset_t index(const T& val)const
    {
        auto begin=this->begin(), end=this->end();
        auto it=std::find(begin, end, val);
        if(it==end)
            throw std::invalid_argument("The given value doesn't exist in this sequence.");
        return it-begin;
    }

    /** insert value at offset i.
     * @param i the offset, like python, -1 means the last position.
     * @param val the value.
     * @throw proton::err if i is bad.
     */
    void insert(offset_t i, const T& val)
    {
        auto it=this->begin()+ offset(i);
        baseT::insert(it, val);
    }

    void insert(offset_t i, T&& val)
    {
        auto it=this->begin()+offset(i);
        baseT::insert(it, val);
    }

    /** pop an item from the sequence.
     * @param i the index of the itme, default is -1, the last item.
     * @return the value at offset i.
     * @throw proton::err if i is bad.
     */
    T pop(offset_t i=-1)
    {
        auto it=this->begin()+offset(i);
        T r=*it;
        this->erase(it);
        return r;
    }

    /** remove the first occurence of a value.
     * @param val the value.
     * @throw std::invalid_argument if there is no such a value.
     */
    void remove(const T& val)
    {
        auto begin=this->begin(), end=this->end();
        auto it=std::find(begin, end, val);
        if(it==end)
            throw std::invalid_argument("The given value doesn't exist in this sequence.");
        this->erase(it);
    }

    /** reverses the items in place.
     */
    void reverse()
    {
        std::reverse(this->begin(), this->end());
    }

    /** sort the items in place.
     */
    void sort()
    {
        std::sort(this->begin(), this->end());
    }

    /** sort the items in place using a customized cmp.
     */
    template<class cmpT>void sort(cmpT cmp)
    {
        std::sort(this->begin(), this->end(),cmp);
    }

};

/**
 * @example vector.cpp
 */

/** vector + vector
 */
template<typename T, typename A, typename X>
vector_<T,A> operator+(const std::vector<T,A>& s, X&& t)
{
    vector_<T,A> r(s);
    r.extend(std::forward<X>(t));
    return r;
}

template<typename T, typename A, typename X>
vector_<T,A> operator+(std::vector<T,A>&& s, X&& t)
{
    vector_<T,A> r(s);
    r.extend(std::forward<X>(t));
    return r;
}

/** vector_ * n
 */
template<typename T, typename A>
vector_<T,A> operator*(const std::vector<T,A>& s, size_t n)
{
    vector_<T,A> r;
    r.reserve(s.size()*n);
    for(size_t i=0; i<n; i++)
        r.extend(s);
    return r;
}

/** n * vector_
 */
template<typename T, typename A>
vector_<T,A> operator*(size_t n, const std::vector<T,A>& s)
{
    return s*n;
}

/** cast to proton::vector_<>& from std::vector<>&.
 */
template<typename T, typename A>
vector_<T,A>& cast_(std::vector<T,A>& x)
{
    return reinterpret_cast<vector_<T,A>&>(x);
}

template<typename T, typename A>
const vector_<T,A>& cast_(const std::vector<T,A>& x)
{
    return reinterpret_cast<const vector_<T,A>&>(x);
}

template<typename T, typename A>
vector_<T,A>&& cast_(std::vector<T,A>&& x)
{
    return reinterpret_cast<vector_<T,A>&&>(x);
}

template<typename T, typename A>
const vector_<T,A>&& cast_(const std::vector<T,A>&& x)
{
    return reinterpret_cast<const vector_<T,A>&&>(x);
}

/**
 * @}
 */
}

#endif // PROTON_VECTOR_HEADER
