#ifndef PROTON_H
#define PROTON_H

#include <type_traits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <string>
#include <list>
#include <map>
#include <string.h>

namespace proton{

void log_open(const char* orig_fn, bool add_pid=false);
void log_close();
void log_console(bool flag);

extern int debug_level; ///< for LOG() macro
extern int output_level; ///< 0: no output to file, 1: output
extern bool is_log_console; ///< false: no console output, 1: output
extern int wait_on_err; ///< 0: nonstop, 1: stop on ERR, 2: stop on THROW_IF assert

#ifndef DEBUG_OPT
#define DEBUG_OPT 1
#endif

#if DEBUG_OPT
#define LOG( lvl, out )\
    {\
        if ( (lvl) <= debug_level) {\
            time_t ti=time(NULL);\
            char* t=ctime(&ti);\
            char* p=strchr(t, '\n');\
            if(p)\
                *p='\0';\
            if(proton::is_log_console)\
                std::cerr << "[" << t << "] " << "LOG " \
                    << __filename(__FILE__) << ":" << __LINE__ << " : \t" << out << std::endl;\
            if(proton::output_level>0){\
                if(proton::g_logger.is_open()){\
                    try{\
                        proton::g_logger << "[" << t << "] "  << "LOG " \
                            << __filename(__FILE__) << ":" << __LINE__ << " : \t" << out << std::endl;\
                        proton::g_logger.flush();\
                    }\
                    catch(...)\
                    {\
                        std::cerr << "[" << t << "] " << "An exception happened when writing log !!!" << std::endl;\
                    }\
                }\
                else{\
                    if(proton::log_warn()){\
                        std::cerr << "[" << t << "] " << "The log file is not opened !!!" << std::endl;\
                    }\
                }\
            }\
        }\
    }
#define THROW_IF(cond, out)\
    {\
        if (cond) {\
            time_t ti=time(NULL);\
            char* t=ctime(&ti);\
            char* p=strchr(t, '\n');\
            if(p)\
                *p='\0';\
            if(proton::is_log_console)\
                std::cerr << "[" << t << "] " << "ERR " \
                    << __filename(__FILE__) << ":" << __LINE__ << " : \t" << out << std::endl;\
            if(proton::output_level>0){\
                if(proton::g_logger.is_open()){\
                    try{\
                        proton::g_logger << "[" << t << "] "  << "ERR " \
                            << __filename(__FILE__) << ":" << __LINE__ << " : \t" << out << std::endl;\
                        proton::g_logger.flush();\
                    }\
                    catch(...)\
                    {\
                        std::cerr << "[" << t << "] " << "An exception happened when writing log !!!" << std::endl;\
                    }\
                }\
                else{\
                    if(proton::log_warn()){\
                        std::cerr << "[" << t << "] " << "The log file is not opened !!!" << std::endl;\
                    }\
                }\
            }\
            if(proton::wait_on_err>=2){\
                std::cout << "Assert failed. Waiting, press any key to continue..." << std::endl;\
                getchar();\
            }\
            throw proton::err("assert");\
        }\
    }
#else
#define LOG( lvl, out )
#define THROW_IF(cond, out)
#endif

#define ERR(out)\
    {\
        time_t ti=time(NULL);\
        char* t=ctime(&ti);\
        char* p=strchr(t, '\n');\
        if(p)\
            *p='\0';\
        if(proton::is_log_console)\
            std::cerr << "[" << t << "] " << "ERR " \
                << __filename(__FILE__) << ":" << __LINE__ << " : \t" << out << std::endl;\
        if(proton::output_level>0){\
            if(proton::g_logger.is_open()){\
                try{\
                    proton::g_logger << "[" << t << "] "  << "ERR " \
                        << __filename(__FILE__) << ":" << __LINE__ << " : \t" << out << std::endl;\
                    proton::g_logger.flush();\
                }\
                catch(...)\
                {\
                    std::cerr << "[" << t << "] " << "An exception happened when writing log !!!" << std::endl;\
                }\
            }\
            else{\
                if(proton::log_warn()){\
                    std::cerr << "[" << t << "] " << "The log file is not opened !!!" << std::endl;\
                }\
            }\
        }\
        if(proton::wait_on_err>=1){\
            std::cout << "An error has happened. Waiting, press any key to continue..." << std::endl;\
            getchar();\
        }\
        throw proton::err("err");\
    }

// exception class
class err{
protected:
    std::string _msg;
public:
    err(const char* msg):_msg(msg)
    {}

    const std::string& to_str()const
    {
        return _msg;
    }
};

inline std::ostream& operator<<(std::ostream& o, const err& e)
{
    o << e.to_str();
    return o;
}

// inner use
extern std::ofstream g_logger;
bool log_warn();
void __log(const char* msg);
inline const char* __filename(const char* pathname)
{
    const char* p = strrchr(pathname, '\\');
    if(!p)
        p = strrchr(pathname,'/');
    if(!p)
        return pathname;
    else
        return p;
}

/////////////////////////////
/// unittest

///@return 0 - success
typedef int (*unittest_t)();

int unittest_run(unittest_t ut[]);

////////////////////////////////////////////////////////////////////////////
// misc

/**
 * return:
 *   int, 0-success, !0-error
 */
int getopt(std::map<char, std::string>& optlist, std::list<std::string>& arglist,
           int argc, char* const argv[],
           const std::string& optstr);

#if 0

///warning: temporary objects are not safe!

template<typename C, bool is_const>struct __iterator_t{};

template<typename C>struct __iterator_t<C,true>{
    typedef typename C::const_iterator type;
};

template<typename C>struct __iterator_t<C,false>{
    typedef typename C::iterator type;
};

template<typename C>class each_iter_t {
    typedef typename __iterator_t<C,std::is_const<C>::value >::type iter_t;
    typedef typename iter_t::pointer pointer;
    typedef typename iter_t::reference reference;

protected:
    iter_t end;

public:
    iter_t cur;

    each_iter_t(C& x):end(x.end()),cur(x.begin())
    {}

    each_iter_t(const each_iter_t& x):end(x.end),cur(x.cur)
    {}

    bool is_over()
    {
        return cur==end;
    }

    bool operator==(const each_iter_t& x)const
    {
        return cur==x.cur;
    }

    bool operator==(const iter_t& x)const
    {
        return cur==x;
    }

    each_iter_t& operator++()
    {
        ++cur;
        return *this;
    }

    each_iter_t operator++(int)
    {
        each_iter_t x=*this;
        ++cur;
        return x;
    }

    reference operator*()const
    {
        return *cur;
    }

    pointer operator->()const
    {
        return &(*cur);
    }

};

template<typename C>each_iter_t<const C> get_each_iter(const C& x)
{
    each_iter_t<const C> it(x);
    return it;
}

template<typename C>each_iter_t<C> get_each_iter(C& x)
{
    each_iter_t<C> it(x);
    return it;
}

#define each(it, list)\
	for(auto it=get_each_iter(list); !it.is_over(); ++it)

#else

template<typename C>auto get_begin(C& x) ->decltype(x.begin())
{
    return x.begin();
}

///in case using temporary objects which are not safe!

#define each(it, list)\
	for(auto it=get_begin(list), end=list.end(); it!=end; ++it)

#endif

/*
template<typename C, typename T> each_iter_t<C>& operator+=(each_iter_t<C>& x, T off)
{
    x.__cur+=off;
    return x;
}

template<typename C, typename T> each_iter_t<C>& operator-=(each_iter_t<C>& x, T off)
{
    x.__cur-=off;
    reutrn x;
}

template<typename C, typename T> each_iter_t<C> operator+(each_iter_t<C>& x, T off)
{
    each_iter_t<C> y(x);
    y.__cur+=off;
    return y;
}

template<typename C, typename T> each_iter_t<C> operator+(each_iter_t<C>& x, T off)
{
    each_iter_t<C> y(x);
    y.__cur+=off;
    return y;
}
*/


}; // namespace proton

#endif // PROTON_H
