#ifndef PROTON_VECTOR_HEADER
#define PROTON_VECTOR_HEADER

#include <vector>
#include <algorithm>
#include <iostream>

namespace proton{

template <typename T, typename allocT, typename V >
bool has(const std::vector<T,allocT>& x, V&& val)
{
    return std::find(x.begin(), x.end(), val)!=x.end();
}

template <typename T, typename A, typename V>
std::vector<T,A>& operator<<(std::vector<T,A>& x, V&& val)
{
    x.push_back(val);
    return x;
}

template <typename T, typename A, typename V>
std::vector<T,A>& operator>>(std::vector<T,A>& x, V& val)
{
    PROTON_THROW_IF(x.empty(), "want to pop an empty vector.");
    val=x.back();
    x.pop_back();
    return x;
}

template <typename T, typename A>
std::ostream& operator<<(std::ostream& s, std::vector<T,A>& x)
{
    s << "[";
    bool first=true;
    each(it, x){
        if(first)
            first=false;
        else
            s <<", ";
        s << *it;
    }
    s << "]";
    return s;
}

/*
template <typename T, typename A>
bool operator==(const std::vector<T,A>& x, const std::vector<T,A>& y)
{
    if(x.size()!=y.size())
        return false;
    auto it_y=y.begin();
    each(it_x,x){
        if(*it_x!=*it_y)
            return false;
        ++it_y;
    }
    return true;
}

template <typename T, typename A>
bool operator!=(const std::vector<T,A>& x, const std::vector<T,A>& y)
{
    if(x.size()!=y.size())
        return true;
    auto it_y=y.begin();
    each(it_x,x){
        if(*it_x!=*it_y)
            return true;
        ++it_y;
    }
    return false;
}
*/

template <typename T, typename A>
void sort(std::vector<T,A>& x)
{
    std::sort(x.begin(), x.end());
}

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

}

#endif // PROTON_VECTOR_HEADER
