import getopt, sys

def count_file(fn, cb, cw, cl):
    total_bytes=0
    total_words=0
    total_lines=0

    f=file(fn)
    for line in f:
        total_lines+=1
        words=line.split()
        total_words+=len(words)
        total_bytes=f.tell()
    return total_bytes, total_words, total_lines

try:
    opts,args=getopt.getopt(sys.argv[1:], "cwl", ["bytes","lines","words","help"])
except:
    print "bad arg"
    sys.exit(-1)

count_bytes=True
count_lines=True
count_words=True

if len(opts)>0:
    count_bytes=False
    count_lines=False
    count_words=False

for x in opts:
    s=x[0]
    if(s=='-c' or s=='--bytes'):
        count_bytes=True
    elif (s=='-w' or s=='--words'):
        count_words=True
    elif (s=='-l' or s=='--lines'):
        count_lines=True
    elif (s=='--help'):
        print 'help'
        sys.exit(0)

for x in args:
    bytes, words, lines=count_file(x, count_bytes, count_words, count_lines)
    if count_lines:
        print lines,
    if count_words:
        print words,
    if count_bytes:
        print bytes,
    print x

sys.exit(0)
