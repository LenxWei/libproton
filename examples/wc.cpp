#include <proton/base.hpp>
#include <proton/getopt.hpp>
#include <proton/io.hpp>
#include <proton/string.hpp>

using namespace std;
using namespace proton;

void usage()
{
    cout <<
"Usage: wc [OPTION]... [FILE]...\n"
"  -c, --bytes            print the byte counts\n"
"  -l, --lines            print the newline counts\n"
"  -w, --words            print the word counts\n"
"      --help     display this help and exit\n"
    << endl;
}

tuple<long, long, long> count_file(Str fn, bool cb, bool cw, bool cl)
{
    long total_bytes=0, total_words=0, total_lines=0;

    File f(fn->c_str());
    Str line(alloc);
    while(readline(*line, *f)){
        total_lines++;
        auto words=line->Split();
        total_words+=len(words);
        total_bytes=f->tellg();
    }
    return _t(total_bytes,total_words,total_lines);
}

int main(int argc, char** argv)
{
    vector_<tuple<str, str> > opts;
    vector_<str> args;
    try{
        tie(opts, args)=getopt(argc, argv, "clw", {"bytes","lines","words","help"});
    }
    catch(std::invalid_argument& e){
        usage();
        return -1;
    }

    bool count_bytes=true;
    bool count_lines=true;
    bool count_words=true;

    if(len(opts)>0){
        count_bytes=false;
        count_lines=false;
        count_words=false;
    }

    for(auto x : opts){
        auto s=get<0>(x);
        if(s=="-c" || s=="--bytes"){
            count_bytes=true;
        }
        else if(s=="-w" || s=="--words"){
            count_words=true;
        }
        else if(s=="-l" || s=="--lines"){
            count_lines=true;
        }
        else if(s=="--help"){
            usage();
            return 0;
        }
    }

    for(auto x:args){
        auto r=count_file(Str(x), count_bytes, count_words, count_lines);
        cout << " " ;
        if(count_lines)
            cout << get<2>(r) << " ";
        if(count_words)
            cout << get<1>(r) << " ";
        if(count_bytes)
            cout << get<0>(r) << " ";
        cout << x << endl;
    }
    return 0;
}

