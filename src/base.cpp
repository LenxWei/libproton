#include "proton/base.hpp"
#include "proton/detail/unit_test.hpp"
#include <map>
#include <list>
#include <string>

using namespace std;

namespace proton {

int debug_level = 0;
bool log_console=true;
int wait_on_err = 2;

class init_alloc{} alloc;

int detail::unittest_run(vector<unittest_t>& ut)
{
    int i=-1, r, fail = 0, fatal=0;
    unittest_t f;

    //SCPT_RT_D_STDERR_OUTPUT = 0;
    for(auto f:ut){
        i++;
        std::cout << "\n>>> test case : " << i << std::endl;
        try{
            if((r=f())!=0){
                fail++;
                std::cout << "\n! case " << i << ", failed, errno : " << r << std::endl;
            }
        }
        catch(const err& e){
            fail++;
            std::cout << "\n! case " << i << ", " << e << std::endl;
            continue;
        }
        catch(...){
            fail++;
            fatal++;
            std::cout << "\n! uncaught fatal err !" << std::endl;
            continue;
        }
        std::cout << "-> success" << std::endl;
    }

    std::cout << "\n>>> end, total : " << i+1 << ", fail : " << fail
         << ", fatal : " << fatal << std::endl;

    if(fail==0 && fatal ==0)
        return 0;
    else
        return -1;
}

////////////////////////////////////////////////////////////////////////////
// misc

/**
 * return:
 *   int, 0-success, !0-error
 *
 */
int getopt(map<char, string>& optlist, list<string>& arglist,
           int argc, char* const argv[],
           const string& optstr)
{
    // parsing optstr to opt_dict
    map<char, int> opt_dict;
    char key;
    long i, j, len=optstr.size();
    for(i=0; i < len; ++i){
        key=optstr[i];
        if(i<len-1 && optstr[i+1]==':'){
            opt_dict[key]=1;
            ++i;
        }
        else
            opt_dict[key]=0;
    }

    optlist.clear();
    arglist.clear();
    for(i=1; i < argc; ++i){
        len=strlen(argv[i]);
        if(len==0)
            continue;
        else if(len==1 || argv[i][0]!='-'){
            arglist.push_back(argv[i]);
            continue;
        }
        else if(argv[i][1]=='-'){
            //LOG(0, "Long options are not supported yet!");
            return -3;
        }
        else{
            for(j=1;j<len;++j){
                key=argv[i][j];
                if(opt_dict.find(key)==opt_dict.end())
                    return -1;
                if(opt_dict[key]){// have parameter
                    if(j<len-1){
                        optlist[key]=string(argv[i]+j+1);
                        break;
                    }
                    else if(i<argc-1){
                        optlist[key]=string(argv[i+1]);
                        ++i;
                        break;
                    }
                    else
                        return -2;
                }
                else
                    optlist.insert(make_pair(key, ""));
            }
        }
    }
    return 0;
}

}; //namespace proton

