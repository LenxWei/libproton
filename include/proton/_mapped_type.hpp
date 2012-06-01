#ifndef PROTON_MAPPED_TYPE_HEADER
#define PROTON_MAPPED_TYPE_HEADER

namespace proton{

template <typename mapT, typename K>
typename mapT::mapped_type get(const mapT& x, K&& key)
{
    auto it=x.find(key);
    THROW_IF(it==x.end(), "can't find the key in a map");
    return it->second;
}

template <typename mapT, typename K, typename V>
typename mapT::mapped_type get(const mapT& x, K&& key, V&& dft)
{
    auto it=x.find(key);
    if(it==x.end())
        return dft;
    return it->second;
}

template <typename mapT, typename K>
bool test_get(typename mapT::mapped_type& v, const mapT& x, K&& key)
{
    auto it=x.find(key);
    if(it==x.end())
        return false;
    v=it->second;
    return true;
}

template <typename mapT, typename K, typename V>
bool test_insert(mapT& x, K&& k, V&& v)
{
    return x.insert({k,v}).second;
}

template <typename mapT, typename K, typename V>
bool test_insert_or_get(mapT& x, K&& k, V& v)
{
    auto p=x.insert({k,v});
    if(p.second)
        return true;
    v=p.first->second;
    return false;
}

template <typename mapT, typename K, typename ...argT>
typename mapT::mapped_type& get_or_create(bool& hit, mapT& x, K&& k, argT&& ... v)
{
    auto p=x.find(k);
    if(p!=x.end()){
        hit= true;
        return p->second;
    }
    hit=false;
#if 0
    p=x.emplace(k, v...);
#else
    p=x.insert({k,typename mapT::mapped_type(v...)}).first;
#endif
    return p->second;
}

}

#endif // PROTON_MAPPED_TYPE_HEADER
