#ifndef PROTON_GETOPT_H
#define PROTON_GETOPT_H

#include <proton/vector.hpp>
#include <proton/string.hpp>
#include <proton/tuple.hpp>
#include <stdexcept>

namespace proton{

/** @addtogroup getopt
 * @{
 */

/** return type of getopt().
 */
typedef std::tuple<vector_<std::tuple<str, str> >, vector_<str> > getopt_t;

/** like getopt in python.
 * Note: the prototype above is wrong due to some bugs of doxygen,
 * use the prototype in the function list (above the "detailed description" line).
 * @param argc    argc in main()
 * @param argv    argv in main()
 * @param optstr  the option string
 * @param longopt the long option arrays
 * @return ([(option, value)], [args])
 * @throw std::invalid_argument
 */
getopt_t getopt(
       int argc, char* const argv[],
       const str& optstr, const vector_<str>& longopt={});

/**
 * @example getopt.cpp
 * @}
 */

} // ns proton

#endif // PROTON_GETOPT_H
