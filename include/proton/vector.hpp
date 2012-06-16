#ifndef PROTON_VECTOR_HEADER
#define PROTON_VECTOR_HEADER


/** @file vector.hpp
 *  @brief vector support.
 *  Please include this header instead of \<vector\>.
 */

#include <vector>
#include <algorithm>
#include <iostream>

namespace proton{

/** @addtogroup vector_
 * @{
 */

/** check if x has val.
 * @param x the container
 * @param val the value
 * @return true: has, false: not
 */
template <typename T, typename allocT, typename V >
bool has(const std::vector<T,allocT>& x, V&& val)
{
    return std::find(x.begin(), x.end(), val)!=x.end();
}

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

/** sort a vector
 * @param x the vector to be sorted
 */
template <typename T, typename A>
void sort(std::vector<T,A>& x)
{
    std::sort(x.begin(), x.end());
}

/** get an item like python.
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

/** get a const item like python.
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

/** get a slice like python.
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

/** get a slice like python.
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

template <typename T, typename A>
struct vector_ : public std::vector<T,A>{
public:
    typedef std::vector<T,A> baseT;

    template<typename ...argT> vector_(argT&& ...a):baseT(a...)
    {}

    void append(T&& x)
    {
        push_back(x);
    }

};

/**
 * @}
 */
}

#endif // PROTON_VECTOR_HEADER
