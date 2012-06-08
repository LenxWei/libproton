#ifndef PROTON_LIST_HEADER
#define PROTON_LIST_HEADER

#include <list>
#include <iterator>
#include <algorithm>
#include <iostream>

namespace proton{

template <typename T, typename allocT, typename V >
bool has(const std::list<T,allocT>& x, V&& val)
{
    return std::find(x.begin(), x.end(), val)!=x.end();
}

template <typename T, typename A, typename V>
std::list<T,A>& operator<<(std::list<T,A>& x, V&& val)
{
    x.push_back(val);
    return x;
}

template <typename T, typename A, typename V>
std::list<T,A>& operator>>(std::list<T,A>& x, V& val)
{
    PROTON_THROW_IF(x.empty(), "want to pop an empty list.");
    val=x.back();
    x.pop_back();
    return x;
}

template <typename T, typename A>
std::ostream& operator<<(std::ostream& s, std::list<T,A>& x)
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
bool operator==(const std::list<T,A>& x, const std::list<T,A>& y)
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
bool operator!=(const std::list<T,A>& x, const std::list<T,A>& y)
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
void sort(std::list<T,A>& x)
{
    x.sort();
}

template <typename T, typename A>
T& get(std::list<T,A>& x, long i)
{
    if(i>=0){
        long j=0;
        auto it=x.begin(),end=x.end();
        for(; j<i; j++,++it){
            if(it==end){
                break;
            }
        }
        if(it==end){
            PROTON_ERR("out of range: look up "<<i<<" in a list whose size is " << j);
        }
        return *it;
    }
    else{
        long j=0;
        auto it=x.end(),begin=x.begin();
        for(; j>i; --j,--it){
            if(it==begin){
                PROTON_ERR("out of range: look up "<<-j+i<<" in a list whose size is " << -j);
            }
        }
        return *it;
    }
}

template <typename T, typename A>
const T& get(const std::list<T,A>& x, long i)
{
    if(i>=0){
        long j=0;
        auto it=x.begin(),end=x.end();
        for(; j<i; j++,++it){
            if(it==end){
                break;
            }
        }
        if(it==end){
            PROTON_ERR("out of range: look up "<<i<<" in a list whose size is " << j);
        }
        return *it;
    }
    else{
        long j=0;
        auto it=x.end(),begin=x.begin();
        for(; j>i; --j,--it){
            if(it==begin){
                PROTON_ERR("out of range: look up "<<-j+i<<" in a list whose size is " << -j);
            }
        }
        return *it;
    }
}


template <typename T, typename A>
std::list<T,A> sub(const std::list<T,A>& x, long i)
{
    typename std::list<T,A>::const_iterator it;
    if(i>=0){
        long j=0;
        it=x.begin();
        auto end=x.end();
        for(; j<i; j++,++it){
            if(it==end){
                return {};
            }
        }
        if(it==end){
            return {};
        }
        // it points to the first item
    }
    else{
        long j=0;
        it=x.end();
        auto begin=x.begin();
        for(; j>i; --j,--it){
            if(it==begin){
                return x;
            }
        }
    }

    std::list<T,A> r;
    std::copy(it, x.end(), std::back_inserter(r));
    return r;
}

template <typename T, typename A>
std::list<T,A> sub(const std::list<T,A>& x, long first, long last)
{
    bool diff_sign=true;
    if(first>=0 && last>=0){
        if(first>=last)
            return {};
        diff_sign=false;
    }
    if(first<0 && last<0){
        if(first>=last)
            return {};
        diff_sign=false;
    }

    auto end=x.end(), begin=x.begin();
    typename std::list<T,A>::const_iterator it;
    long i=first;
    long size=-1;
    if(i>=0){
        long j=0;
        it=begin;
        for(; j<i; j++,++it){
            if(it==end){
                return {};
            }
        }
        if(it==end){
            return {};
        }
        // it points to the first item
    }
    else{
        long j=0;
        it=end;
        for(; j>i; --j,--it){
            if(it==begin){
                size=-j;
                break;
            }
        }
    }

    std::list<T,A> r;
    if(!diff_sign){
        if(first<0 && size>=0){
            last=last+size;
            if(last<=0)
                return r;
            first=first+size;
            if(first<0)
                first=0;
        }
        for(long i=last-first;i>0;--i,++it){
            if(it!=end)
                r << *it;
            else
                break;
        }
        return r;
    }

    typename std::list<T,A>::const_iterator it1;
    i=last;
    if(i>=0){
        bool pass=false;
        long j=0;
        it1=begin;
        for(; j<i; j++,++it1){
            if(it1==it)
                pass=true;
            if(it1==end){
                break;
            }
        }
        if(!pass)
            return r;
        // it1 points to the last item
    }
    else{
        long j=0;
        it1=end;
        for(; j>i; --j,--it1){
            if(it1==it)
                return r;
        }
        // it1 points to the first item
    }

    std::copy(it,it1, std::back_inserter(r));
    return r;
}

}

#endif // PROTON_LIST_HEADER
