#ifndef PROTON_STRING_HEADER
#define PROTON_STRING_HEADER

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>

#ifndef __linux__
#define strncasecmp strnicmp
#define strcasecmp _stricmp
#endif

namespace proton{

template<typename string>string strip(const string& x)
{
    string spc = " \t\n\r";

    long i=x.find_first_not_of(spc);
    long j=x.find_last_not_of(spc);

    if(i<0 || j<0 || j < i )
        return "";

    return x.substr(i,j-i+1);
}

/// @param null_unite: -1: a.c.t. python depent on token, 0: false, 1: true
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

template<typename string> bool startswith(const string& s, const string& sub)
{
    long s0=s.length(), s1=sub.length();
    if(s0<s1)
        return false;

    return (s.compare(0, s1, sub)==0);
}

template<typename string> bool istartswith(const string& s, const char* sub)
{
    long s0=s.length(), s1=strlen(sub);
    if(s0<s1)
        return false;

    return (strncasecmp(s.c_str(), sub, s1)==0);
}

template<typename string> bool startswith(const string& s, const char* sub)
{
    long s0=s.length(), s1=strlen(sub);
    if(s0<s1)
        return false;

    return (s.compare(0, s1, sub, s1)==0);
}

template<typename string> bool endswith(const string& s, const string& sub)
{
    long s0=s.length(), s1=sub.length();
    if(s0<s1)
        return false;

    return (s.compare(s0-s1, s1, sub)==0);
}

template<typename string> bool endswith(const string& s, const char* sub)
{
    long s0=s.length(), s1=strlen(sub);
    if(s0<s1)
        return false;

    return (s.compare(s0-s1, s1, sub, s1)==0);
}

template<typename string> bool iendswith(const string& s, const char* sub)
{
    long s0=s.length(), s1=strlen(sub);
    if(s0<s1)
        return false;

    return (strncasecmp(s.c_str()+s0-s1, sub, s1)==0);
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

template<typename string> string to_(long long n, int base=10)
{
    std::basic_ostringstream<char, std::char_traits<char>, typename string::allocator_type > s;
    set_base(s, base, true);
    s << n;
    return s.str();
}

template<typename int_t, typename string> bool get_int(int_t& r, const string& s, int base=10)
{
    bool is_hex= (base==16);
    string v1;
    if( endswith(s,"h") ){
        v1=s.substr(0,s.length()-1);
        is_hex=true;
    }
    else if( startswith(s,"0x") ){
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

/*
template<typename string> bool get_unsigned(unsigned long& r, const string& s, int base=10)
{
    bool is_hex= (base==16);
    string v1;
    if( endswith(s,"h") ){
        v1=s.substr(0,s.length()-1);
        is_hex=true;
    }
    else if( startswith(s,"0x") ){
        v1=s;
        is_hex=true;
    }
    else{
        v1=s;
    }
    std::basic_istringstream<char, std::char_traits<char>, typename string::allocator_type >
        i(v1);
    long long unsigned x;
    if(is_hex)
        i >> std::hex;
    if ( (!(i >> x)) || (i.get()>0) ){
        return false;
    }
    r=(unsigned long)x;
    return true;
}
*/

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

}

#endif // PROTON_STRING_HEADER
