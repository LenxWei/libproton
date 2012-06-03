#ifndef PROTON_H
#define PROTON_H

#include <iostream>
#include <time.h>
#include <string.h>
#include <iomanip>
#include <stdio.h>

namespace proton{

extern int debug_level; ///< for LOG() macro
extern bool log_console; ///< false: no console output, true: output
extern int wait_on_err; ///< 0: nonstop, 1: stop on ERR, 2: stop on THROW_IF assert

namespace detail{

inline const char* filename(const char* pathname)
{
    const char* p = strrchr(pathname, '\\');
    if(!p)
        p = strrchr(pathname,'/');
    if(!p)
        return pathname;
    else
        return p;
}

inline void output_ts(std::ostream& o, const char* type, const char* fn, long ln)
{
    std::ios_base::fmtflags ff=o.flags();
    time_t now;
    time(&now);
    struct tm* ti=localtime(&now);
    o << std::dec << std::right << "[";
    o << ti->tm_year+1900 << "-"
      << ti->tm_mon+1 << "-" << ti->tm_mday;
    o << " " << std::setw(2) << ti->tm_hour << ":" << std::setw(2) << ti->tm_min
      << ":" << std::setw(2) << ti->tm_sec << "] "
      << type << " " <<  filename(fn) << "#" << ln;
    o.flags(ff);
}

}

#ifndef PROTON_DEBUG_OPT
#define PROTON_DEBUG_OPT 1
#endif

#if PROTON_DEBUG_OPT
#define PROTON_LOG( lvl, out )\
    {\
        if ( proton::log_console && (lvl) <= proton::debug_level ) {\
            proton::detail::output_ts(std::cerr, "LOG", __FILE__, __LINE__);\
            std::cerr << " : " << out << std::endl;\
        }\
    }
#define PROTON_THROW_IF(cond, out)\
    {\
        if (cond) {\
            if(proton::log_console){\
                proton::detail::output_ts(std::cerr, "BAD", __FILE__, __LINE__);\
                std::cerr << " : " << out << std::endl;\
            }\
            if(proton::wait_on_err>=2){\
                std::cout << "Assert failed. Waiting, press any key to continue..." << std::endl;\
                ::getchar();\
            }\
            throw proton::err("assert");\
        }\
    }
#else
#define PROTON_LOG( lvl, out )
#define PROTON_THROW_IF(cond, out)
#endif

#define PROTON_ERR(out)\
    {\
        if(proton::log_console){\
            proton::detail::output_ts(std::cerr, "ERR", __FILE__, __LINE__);\
            std::cerr << " : " << out << std::endl;\
        }\
        if(proton::wait_on_err>=1){\
            std::cout << "An error has happened. Waiting, press any key to continue..." << std::endl;\
            ::getchar();\
        }\
        throw proton::err("err");\
    }

// exception class
class err: public std::exception{
protected:
    const char* _msg;
public:
    err(const char* msg) throw():_msg(msg)
    {}

    err(const err& e) throw():_msg(e._msg)
    {}

    err& operator=(const err& e) throw()
    {
        _msg=e._msg;
    }

    const char* what()const throw()
    {
        return _msg;
    }
};

inline std::ostream& operator<<(std::ostream& o, const err& e)
{
    o << e.what();
    return o;
}

}; // namespace proton

#endif // PROTON_H
