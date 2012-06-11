#ifndef HANDY_TMP_TYPES_HEADER
#define HANDY_TMP_TYPES_HEADER

#include <string>
#include <map>
#include <set>
#inlcude <deque>
#include <vector>

#include <proton/pool.hpp>

namespace proton{

#define meta_allocator smart_allocator

typedef std::basic_string<char, std::string::traits_type,
            smart_allocator<char, tmp_pool> > tstring;

template<typename string1, typename A>
    string1 to_( const std::basic_string<char, std::string::traits_type, A>& s )
    {
        return string1(s.c_str(), s.size());
    }

#define tvector(T) std::vector<T, meta_allocator<T, tmp_pool> >
#define tdeque(T) std::deque<T, meta_allocator<T, tmp_pool> >
#define tlist(T) std::list<T, meta_allocator<T, tmp_pool> >
#define tmap(K,T) std::map<K, T, std::less<K >, meta_allocator<std::pair<const K, T >, tmp_pool> >
#define tset(K) std::set<K, std::less<K >, meta_allocator<K, tmp_pool> >

typedef std::basic_string<char, std::string::traits_type,
            meta_allocator<char, per_pool> > estring;

#define evector(T) std::vector<T, meta_allocator<T, per_pool> >
#define edeque(T) std::deque<T, meta_allocator<T, per_pool> >
#define elist(T) std::list<T, meta_allocator<T, per_pool> >
#define emap(K,T) std::map<K, T, std::less<K >, meta_allocator<std::pair<const K, T >, per_pool> >
#define eset(K) std::set<K, std::less<K >, meta_allocator<K, per_pool> >

};

#endif //HANDY_TMP_TYPES_HEADER
