#ifndef PROTON_GETOPT_H
#define PROTON_GETOPT_H

namespace proton{

/**
 * return:
 *   int, 0-success, !0-error
 */
int getopt(std::map<char, std::string>& optlist, std::list<std::string>& arglist,
           int argc, char* const argv[],
           const std::string& optstr);

}

#endif // PROTON_GETOPT_H
