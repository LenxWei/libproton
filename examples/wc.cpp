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

tuple<long, long, long> count_file(const str& fn, bool cb, bool cw, bool cl)
{
    long total_bytes=0, total_words=0, total_lines=0;

    fstream f(fn.c_str());
    str line;
    while(len(line=readline(f))){
        total_lines++;
        auto words=line.split();
        total_words+=len(words);
        total_bytes=f.tellg();
    }
    return _t(total_bytes,total_words,total_lines);
}

int main(int argc, char** argv)
{
    getopt_t g;
    try{
        g=getopt(argc, argv, "clw", {"bytes","lines","words","help"});
    }
    catch(invalid_argument& e){
        usage();
        return -1;
    }

    auto opts=at<0>(g);
    auto args=at<1>(g);

    bool count_bytes=true;
    bool count_lines=true;
    bool count_words=true;

    if(len(opts)>0){
        count_bytes=false;
        count_lines=false;
        count_words=false;
    }

    for(auto x : opts){
        auto s=at<0>(x);
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
        long bytes, words, lines;
        tie(bytes, words, lines)=count_file(x, count_bytes, count_words, count_lines);
        cout << " " ;
        if(count_lines)
            cout << lines << " ";
        if(count_words)
            cout << words << " ";
        if(count_bytes)
            cout << bytes << " ";
        cout << x << endl;
    }

    return 0;
}

