#include <proton/base.hpp>
#include <proton/getopt.hpp>

using namespace std;
using namespace proton;

void usage()
{
    cout << "Example of getopt. Usage:" << endl;
    cout <<
" -a            a short option without parameter\n"
" -b par        a short option with parameter\n"
" --clong       a long option without parameter\n"
" --clong1      another long option without parameter\n"
" --dlong=par   a long option with parameter\n"
" -h            show this usage" << endl;
}

int main(int argc, char** argv)
{
    vector_<tuple<str, str> > opts;
    vector_<str> args;
    try{
        tie(opts, args)=getopt(argc, argv, "ab:", {"clong","clong1","dlong="});
    }
    catch(std::invalid_argument& e){
        cerr << e.what() << endl;
        usage();
        return -1;
    }
    cout << "opts :" << opts << endl;
    cout << "args :" << args << endl;
    return 0;
}

