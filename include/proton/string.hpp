#ifndef PROTON_STRING_HEADER
#define PROTON_STRING_HEADER

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <stdexcept>
#include <proton/pool.hpp>
#include <proton/deque.hpp>

namespace proton{

/** @addtogroup str
 * @{
 */

template<typename string>string strip(const string& x)
{
    string spc = " \t\n\r";

    long i=x.find_first_not_of(spc);
    long j=x.find_last_not_of(spc);

    if(i<0 || j<0 || j < i )
        return "";

    return x.substr(i,j-i+1);
}

/** split a string.
 * @param r          the output string list, supporting clear() and push_back()
 * @param s          the input string
 * @param spc        the delimiters
 * @param null_unite -1: a.c.t. python depent on token, 0: false, 1: true
 */
template<typename string_list, typename string> void split(string_list& r, const string& s, string spc="", int null_unite=-1)
{
    long pos = 0, begin, end;

    r.clear();

    if(spc.size()==0){
        if(null_unite<0)
            null_unite=1;
        spc=" \t\n\r";
    }
    else{
        if(null_unite<0)
            null_unite=0;
    }

	if(null_unite){
		do{
			begin = s.find_first_not_of(spc,pos);
			if(begin<0)
				break;
			end = s.find_first_of(spc,begin);
			if(end<0)
				end=s.length();
			r.push_back(s.substr(begin, end-begin).c_str());
			pos = end;
		}
		while(pos < (long)s.length());
	}
	else{
		while(1){
			begin = s.find_first_of(spc,pos);
			if(begin<0){
				r.push_back(s.substr(pos).c_str());
				break;
			}
			r.push_back(s.substr(pos, begin-pos).c_str());
			pos = begin + 1;
            if(pos>=(long)s.length()){
                r.push_back("");
                break;
            }
		}//while
	}//else
}

template<typename string_list, typename string>
    void split(string_list& r, const string& s, const char* spc, int null_unite=-1)
{
    return split(r, s, string(spc), null_unite);
}

/** join a list of strings to one string.
 * @param token the delimiter
 * @param r     the input string list
 * @return the output string
 */
template<typename string_list>
    typename string_list::value_type join(const char* token, const string_list& r)
{
    if(r.empty()){
        return "";
    }
    else{
        typename string_list::const_iterator it=r.begin();
        typename string_list::value_type res=*it;
        ++it;
        for(;it!=r.end();++it){
            res+=token+*it;
        }
        return res;
    }
}

template<typename str1, typename str2> bool startswith(const str1& s,const str2& sub)
{
    return boost::algorithm::starts_with(s, sub);
}

template<typename str1, typename str2> bool istartswith(const str1& s,const str2& sub)
{
    return boost::algorithm::istarts_with(s, sub);
}

template<typename str1, typename str2> bool endswith(const str1& s,const str2& sub)
{
    return boost::algorithm::ends_with(s, sub);
}

template<typename str1, typename str2> bool iendswith(const str1& s,const str2& sub)
{
    return boost::algorithm::iends_with(s, sub);
}

template<typename ostream> void set_base(ostream& s, int base, bool is_num=false)
{
    switch(base){
        case 8:
            s << std::oct;
            if(is_num)
                s<< "0";
            break;
        case 10:
            s << std::dec;
            break;
        case 16:
            s << std::hex;
            if(is_num)
                s<< "0x";
            break;
        default:
            PROTON_LOG(0, "unsupported base : " << base );
    }
}

/** convert a number/object to a string.
 * @param n    the input value
 * @param base 10:dec, 16:hex
 * @return the output string
 */
template<typename string, typename T> string to_(T&& n, int base=10)
{
    std::basic_ostringstream<char, std::char_traits<char>, typename string::allocator_type > s;
    set_base(s, base, true);
    s << n;
    return s.str();
}

/** get integer (including int/long/unsigned and so on) from string.
 * @param r the output value
 * @param s the input string
 * @param base 10:dec, 16:hex
 * @return true: success, false: failure
 */
template<typename int_t, typename string> bool get_int(int_t& r, const string& s, int base=10)
{
    bool is_hex= (base==16);
    string v1;
    if( iendswith(s,"h") ){
        v1=s.substr(0,s.length()-1);
        is_hex=true;
    }
    else if( istartswith(s,"0x") ){
        v1=s;
        is_hex=true;
    }
    else{
        v1=s;
    }
    std::basic_istringstream<char, std::char_traits<char>, typename string::allocator_type >
        i(v1);
    int_t x;
    if(is_hex)
        i >> std::hex;
    if ( (!(i >> x)) || (i.get()>0) ){
        return false;
    }
    r=x;
    return true;
}

template<typename string> string to_lower(const string& s)
{
 	long length = s.size();
    static typename string::allocator_type alloc;
    char* p = alloc.allocate(length);
    if(p){
        char* q = p;
	    const char* p0 = s.c_str();
	    string r;

	    for(long i=0;i<length;++i){
		    *p=tolower(*p0);
            p++;
            p0++;
	    }
	    r = string(q,length);
	    alloc.deallocate(q,length) ;
	    return r;
    }
    else{
        PROTON_ERR("bad alloc:" << length);
    }
}

template<typename string>string to_upper(const string& s)
{
 	long length = s.size();
    static typename string::allocator_type alloc;
    char* p = alloc.allocate(length);
    if(p){
        char* q = p;
	    const char* p0 = s.c_str();
	    string r;

	    for(long i=0;i<length;++i){
		    *p=toupper(*p0);
            p++;
            p0++;
	    }
	    r = string(q,length);
	    alloc.deallocate(q,length) ;

	    return r;
    }
    else{
        PROTON_ERR("bad alloc:" << length);
    }
}

template<typename string>string readline(std::istream& f, char delim='\n')
{
    string r;
    std::ios::pos_type start=f.tellg(), end;
    std::getline(f, r, delim);
    end=f.tellg();
    if(end-start>(long)r.length()){
        char a[2]={delim,0};
        return r+a;
    }
    else
        return r;
}

template <typename C, typename T, typename A>
C get(const std::basic_string<C,T,A>& x, long i)
{
    unsigned long s=x.size();
    if(i<0)
        i=s+i;
    if(i<0 || (unsigned long)i >= s )
        PROTON_ERR("out of range: look up "<<i<<" in vector whose size is " << s);
    return x[i];
}

template <typename C, typename T, typename A>
std::basic_string<C,T,A> sub(const std::basic_string<C,T,A>& x, long first)
{
    unsigned long s=x.size();
    if(first<0)
        first=first+s;
    if(first<0)
        first=0;
    if((unsigned long)first >= s )
        return "";

    return x.substr(first);
}

template <typename C, typename T, typename A>
std::basic_string<C,T,A> sub(const std::basic_string<C,T,A>& x, long first, long last)
{
    unsigned long s=x.size();
    if(first<0)
        first=first+s;
    if(first<0)
        first=0;
    if((unsigned long)first >= s )
        return "";

    if(last<0)
        last=last+s;
    if(last<=0)
        return "";

    if((unsigned long)last>s)
        last=s;
    return x.substr(first,last-first);
}

/** main string template
 */
template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = smart_allocator<CharT>
>
class basic_string_ : public std::basic_string<CharT,Traits,Allocator> {
public:
    typedef std::basic_string<CharT,Traits,Allocator> baseT;
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
        offset_t size=(offset_t)this->size(); //[FIXME] size>2G in 32bit?
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
    template<typename ...argT> basic_string_(argT&& ...a):baseT(a...)
    {}

    /** initializer_list forwarding ctor.
     */
    basic_string_(std::initializer_list<CharT> a):baseT(a)
    {}

    /** copy ctor.
     */
    basic_string_(const basic_string_& x):baseT(x)
    {}

    /** move ctor.
     */
    basic_string_(basic_string_&& x)noexcept:baseT(x)
    {}

    explicit basic_string_(const baseT& x):baseT(x)
    {}

    basic_string_(baseT&& x)noexcept:baseT(x)
    {}

    /** assign.
     */
    basic_string_& operator=(const basic_string_& x)
    {
        baseT::operator=(x);
        return *this;
    }

    basic_string_& operator=(basic_string_&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    basic_string_& operator=(const baseT& x)
    {
        baseT::operator=(x);
        return *this;
    }

    basic_string_& operator=(baseT&& x)noexcept
    {
        baseT::operator=(x);
        return *this;
    }

    template<typename ...argT> basic_string_& operator=(argT&& ...a)
    {
        baseT::operator=(a...);
        return *this;
    }

    basic_string_& operator=(std::initializer_list<CharT> a)
    {
        baseT::operator=(a);
        return *this;
    }

    /** +=
     */
    template<typename argT>
    basic_string_& operator+=(argT&& a)
    {
        this->append(a);
        return *this;
    }

    /** string + string
     */
    template<typename argT>
    basic_string_ operator+(argT&& a)
    {
        basic_string_ r(*this);
        r.append(a);
        return r;
    }

    /** cast to std::vector<>&.
     */
    operator baseT&()
    {
        return reinterpret_cast<baseT&>(*this);
    }

    /** [i] in python
     */
    CharT& operator[](offset_t i)
    {
        return *(this->begin()+offset(i));
    }

    /** [i] in python
     */
    const CharT& operator[](offset_t i)const
    {
        return *(this->begin()+offset(i));
    }

    /** slice of [i:]
     */
    basic_string_ operator()(offset_t i)const
    {
        auto begin=this->begin();
        return basic_string_(begin+offset(i),this->end());
    }

    /** slice of [i:j]
     */
    basic_string_ operator()(offset_t i, offset_t j)const
    {
        auto begin=this->begin();
        fix_range(i,j);
        return basic_string_(begin+i,begin+j);
    }

    /** slice of [i:j:k]
     */
    basic_string_ operator()(offset_t i, offset_t j, size_t k)const
    {
        fix_range(i,j);
        basic_string_ r;
        r.reserve((j-i)/k+2);
        auto it=this->begin()+i;
        for(offset_t n=i; n<j; n+=k,it+=k)
            r.push_back(*it);
        return r;
    }

    /** total number of occurences of a char.
     */
    size_t count(const CharT& x)const
    {
        return std::count(this->begin(), this->end(), x);
    }

    /** index of the first occurence of a char.
     * @param val the char.
     * @throw std::invalid_argument if there is no such a char.
     */
    offset_t index(const CharT& val)const
    {
        auto begin=this->begin(), end=this->end();
        auto it=std::find(begin, end, val);
        if(it==end)
            throw std::invalid_argument("The given char doesn't exist in this sequence.");
        return it-begin;
    }

    /** Return a copy of the string with leading and trailing characters removed.
     * @param spc white space chars
     * @return the stripped string
     */
    basic_string_ strip(const baseT& spc=" \t\n\r")const
    {
        offset_t i=this->find_first_not_of(spc);
        offset_t j=this->find_last_not_of(spc);

        if(i<0 || j<0 || j < i )
            return "";

        return basic_string_(this->begin()+i,this->begin()+j+1);
    }

    /** split a string.
     * @param delim        the delimiters
     * @param null_unite -1: a.c.t. python depent on token, 0: false, 1: true
     * @return the output string list, in deque_<basic_string_>
     */
    deque_<basic_string_ >
        split(const baseT& delim=baseT(), int null_unite=-1)const
    {
        long pos = 0, begin, end;

        deque_<basic_string_> r;

        basic_string_ spc(delim);
        if(spc.size()==0){
            if(null_unite<0)
                null_unite=1;
            spc=" \t\n\r";
        }
        else{
            if(null_unite<0)
                null_unite=0;
        }

        if(null_unite){
            do{
                begin = this->find_first_not_of(spc,pos);
                if(begin<0)
                    break;
                end = this->find_first_of(spc,begin);
                if(end<0)
                    end=this->length();
                r.push_back(this->substr(begin, end-begin));
                pos = end;
            }
            while(pos < (long)this->length());
        }
        else{
            while(1){
                begin = this->find_first_of(spc,pos);
                if(begin<0){
                    r.push_back(this->substr(pos));
                    break;
                }
                r.push_back(this->substr(pos, begin-pos));
                pos = begin + 1;
                if(pos>=(long)this->length()){
                    r.push_back("");
                    break;
                }
            }//while
        }//else
        return r;
    }

    /** join a list of strings to one string.
     * @param r     the input string list
     * @return the output string
     */
    template<typename string_list>
        basic_string_ join(const string_list& r)const
    {
        if(r.empty()){
            return basic_string_();
        }
        else{
            typename string_list::const_iterator it=r.begin();
            basic_string_ res=*it;
            ++it;
            for(;it!=r.end();++it){
                res+=*this;
                res+=*it;
            }
            return res;
        }
    }

    /** startswith.
     */
    template<typename str2> bool startswith(str2&& sub)const
    {
        return boost::algorithm::starts_with(*this, sub);
    }

    /** case-insensitive startswith.
     */
    template<typename str2> bool istartswith(str2&& sub)const
    {
        return boost::algorithm::istarts_with(*this, sub);
    }

    /** endswith.
     */
    template<typename str2> bool endswith(str2&& sub)const
    {
        return boost::algorithm::ends_with(*this, sub);
    }

    /** case-insensitive endswith.
     */
    template<typename str2> bool iendswith(str2&& sub)const
    {
        return boost::algorithm::iends_with(*this, sub);
    }

    /** return a copy with upper case letters converted to lower case.
     */
    basic_string_ lower()const
    {
        return boost::algorithm::to_lower_copy(*this);
    }

    /** return a copy with lower case letters converted to upper case.
     */
    basic_string_ upper()const
    {
        return boost::algorithm::to_upper_copy(*this);
    }

};

/**
 * @example string.cpp
 */

/** string * n
 */
template<typename T, typename C, typename A>
basic_string_<T,C,A> operator*(const std::basic_string<T,C,A>& s, size_t n)
{
    basic_string_<T,C,A> r;
    r.reserve(s.size()*n+1);
    for(size_t i=0; i<n; i++)
        r.append(s);
    return r;
}

/** n * string
 */
template<typename T, typename C, typename A>
basic_string_<T,C,A> operator*(size_t n, const std::basic_string<T,C,A>& s)
{
    return s*n;
}

/** cast to proton::basic_string_<>& from std::basic_string<>&.
 */
template<typename T, typename C, typename A>
basic_string_<T,C,A>& cast_(std::basic_string<T,C,A>& x)
{
    return reinterpret_cast<basic_string_<T,C,A>&>(x);
}

template<typename T, typename C, typename A>
const basic_string_<T,C,A>& cast_(const std::basic_string<T,C,A>& x)
{
    return reinterpret_cast<const basic_string_<T,C,A>&>(x);
}

template<typename T, typename C, typename A>
basic_string_<T,C,A>&& cast_(std::basic_string<T,C,A>&& x)
{
    return reinterpret_cast<basic_string_<T,C,A>&&>(x);
}

template<typename T, typename C, typename A>
const basic_string_<T,C,A>&& cast_(const std::basic_string<T,C,A>&& x)
{
    return reinterpret_cast<const basic_string_<T,C,A>&&>(x);
}

/** the main string type in proton.
 */
typedef basic_string_<char> str;

/** the main wstring type in proton.
 */
typedef basic_string_<wchar_t> wstr;

/**
 * @}
 */

}

#endif // PROTON_STRING_HEADER
