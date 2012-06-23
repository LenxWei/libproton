#ifndef PROTON_GETOPT_H
#define PROTON_GETOPT_H

#include <proton/map.hpp>
#include <proton/vector.hpp>
#include <proton/string.hpp>
#include <proton/tuple.hpp>
#include <stdexcept>

namespace proton{

/** like getopt in python
 * @param argc    argc in main()
 * @param argv    argv in main()
 * @param optstr  the option string
 * @param longopt the long option arrays
 * @return ([(option, value)], [args])
 * @throw std::invalid_argument
 */
std::tuple<vector_<std::tuple<str, str> >, vector_<str> > getopt(
           int argc, char* const argv[],
           const str& optstr, const vector_<str>& longopt={});

} // ns proton

#endif // PROTON_GETOPT_H
