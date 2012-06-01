#include "base.hpp"
#include "string.hpp"

#ifdef _MSC_VER
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#endif

using namespace std;

namespace proton {

int debug_level = 0;
int output_level = 1;
int wait_on_err = 2;

class init_dumb{} dumb;

/////////////////////////////////////////////
// string utils

std::string basename_of(const char* orig_fn)
{
    string fn=orig_fn;
    int dot_pos=fn.find_last_of(".");
    if(dot_pos<=0)
        return fn;
    return fn.substr(0,dot_pos);
}

std::string basename_of(const std::string& orig_fn)
{
    return basename_of(orig_fn.c_str());
}

///////////////////////////////////////////
// log utils

ofstream g_logger;
bool is_log_console=true;
void log_console(bool flag)
{
    is_log_console=flag;
}

void log_open(const char* orig_fn, bool add_pid/*=false*/)
{
    if(output_level>0){
        string log_fn=basename_of(orig_fn)+(add_pid?(string("_")+to_<string>(getpid())):"")+".log";
    #ifdef WIN32
        g_logger.open(log_fn.c_str(),ios::out, _SH_DENYWR);
    #else
        g_logger.open(log_fn.c_str(),ios::out);
    #endif
    }
}

void log_close()
{
    if(output_level>0){
        g_logger.flush();
        g_logger.close();
    }
}

#ifdef _MSC_VER // if using Microsoft C++ compiler
#pragma warning( disable : 4996 ) // disable warning C4200
#endif

bool log_warn()
{
    static bool warn=true;
    if(warn){
        warn=!warn;
    }
    return warn;
}

#if 0
void __log(const char* msg)
{
    time_t ti=time(NULL);
    char* t=ctime(&ti);
    char* p=strchr('\n');
    if(!p)
        *p='\0';

    if(is_log_console)
        cerr << "[" << t << "] " << msg << endl;
    if(output_level>0){
        if(g_logger.is_open()){
            try{
                g_logger << "[" << t << "] " << msg << endl;
                g_logger.flush();
            }
            catch(...)
            {
                cerr << "[" << t << "] " << "An exception happened when writing log !!!" << endl;
            }
        }
        else{
            static bool warn=true;
            if(warn){
                cerr << "[" << t << "] " << "The log file is not opened !!!" << endl;
                warn=false;
            }
        }
    }
}
#endif

int unittest_run(unittest_t ut[])
{
    int i, r, fail = 0, fatal=0;
    unittest_t f;

    //SCPT_RT_D_STDERR_OUTPUT = 0;

    for(i=0; (f=ut[i])!=NULL; i++){
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

    std::cout << "\n>>> end, total : " << i << ", fail : " << fail
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

