#ifndef PROTON_GETOPT_H
#define PROTON_GETOPT_H

#include <proton/map.hpp>
#include <proton/vector.hpp>
#include <proton/string.hpp>

namespace proton{

/** like getopt in python
 * @param opts    output options: {option: value}
 * @param args    output non-option arguments
 * @param argc    argc in main()
 * @param argv    argv in main()
 * @param optstr  the option string
 * @param longopt the long option arrays
 * @return 0: success, !0: error
 */
int getopt(map_<str, str>& opts, vector_<str>& args,
           int argc, char* const argv[],
           const str& optstr, const vector_<str>& longopt={});

} // ns proton

#endif // PROTON_GETOPT_H
