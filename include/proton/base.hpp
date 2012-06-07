#ifndef PROTON_H
#define PROTON_H

/** @file base.hpp
 *  @brief the basic header for Proton Library
 */

#include <iostream>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <cstdio>

/** The main proton namespace.
 */
namespace proton{

extern int debug_level; ///< The level controls output of PROTON_LOG().
extern bool log_console; ///< true: PROTON_LOG/PROTON_THROW_IF/PROTON_ERR will output to console, false: no console output for these macros.
extern int wait_on_err; ///< 0: nonstop, 1: stop on PROTON_ERR, 2: stop on PROTON_THROW_IF assert

/** The implementation namespace of proton.
 * Any thing in this namespace should not be considered as a part of interfaces of Proton.
 */
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
    time_t now=time(NULL);
    struct tm* ti=localtime(&now);
    o << std::dec << std::right << "[";
    o << ti->tm_year+1900 << "-" << ti->tm_mon+1 << "-" << ti->tm_mday;
    o << " " << std::setw(2) << ti->tm_hour << ":" << std::setw(2) << ti->tm_min
      << ":" << std::setw(2) << ti->tm_sec << "] "
      << type << " " <<  filename(fn) << "#" << ln;
    o.flags(ff);
}

}

/** Control debug macros.
 * 1: enable PROTON_LOG/PROTON_THROW_IF/PROTON_ERR, 0: disable them.
 */
#ifndef PROTON_DEBUG_OPT
#define PROTON_DEBUG_OPT 1
#endif

#if PROTON_DEBUG_OPT

/** Log messages to cerr.
 * Output messages when lvl <= proton::debug_level.
 * @param lvl   the log level.
 * @param out   messages to be outputed, they can be concatenated using "<<".
 */
#define PROTON_LOG( lvl, out )\
    {\
        if ( proton::log_console && (lvl) <= proton::debug_level ) {\
            proton::detail::output_ts(std::cerr, "LOG", __FILE__, __LINE__);\
            std::cerr << " : " << out << std::endl;\
        }\
    }

/** Proton's inverse assert.
 * Throw if cond is true, and output the reason.
 * If proton::wait_on_err >= 2, it will hold the program and wait for any key.
 * You can use gdb to attach the process to check what's happened.
 * @param cond  the condition to check, throw proton::err("assert") if it's true.
 * @param out   the message describing what's happened.
 */
#define PROTON_THROW_IF(cond, out)\
    {\
        if (cond) {\
            if(proton::log_console){\
                proton::detail::output_ts(std::cerr, "BAD", __FILE__, __LINE__);\
                std::cerr << " : " << out << std::endl;\
            }\
            if(proton::wait_on_err>=2){\
                std::cout << "Assert failed. Waiting, press any key to continue..." << std::endl;\
                getchar();\
            }\
            throw proton::err("assert");\
        }\
    }
#else
#define PROTON_LOG( lvl, out )
#define PROTON_THROW_IF(cond, out)
#endif

/** Throw an err.
 * Throw a proton::err("err"), and output a message.
 * If proton::wait_on_err >= 1, it will hold the program and wait for any key.
 * You can use gdb to attach the process to check what's happened.
 * @param out   the message describing what's happened.
 */
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

/** The exception class.
 * The exception class used by PROTON_THROW_IF and PROTON_ERR.
 */
class err: public std::exception{
protected:
    const char* _msg;
public:
    /** ctor
     * @param msg the message showed in what().
     */
    err(const char* msg) throw():_msg(msg)
    {}

    err(const err& e) throw():_msg(e._msg)
    {}

    err& operator=(const err& e) throw()
    {
        _msg=e._msg;
        return *this;
    }

    /** get message
     * @return the message used in ctor.
     */
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
