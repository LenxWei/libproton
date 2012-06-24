TESTS = func vector deque string getopt
check_PROGRAMS = func vector deque string getopt

func_SOURCES = functor.cpp
func_CXXFLAGS = $(BOOST_CPPFLAGS)
func_LDADD = $(top_srcdir)/src/libproton.la

string_SOURCES = string.cpp
string_CXXFLAGS = $(BOOST_CPPFLAGS)
string_LDADD = $(top_srcdir)/src/libproton.la

vector_SOURCES = vector.cpp
vector_CXXFLAGS = $(BOOST_CPPFLAGS)
vector_LDADD = $(top_srcdir)/src/libproton.la

deque_SOURCES = deque.cpp
deque_CXXFLAGS = $(BOOST_CPPFLAGS)
deque_LDADD = $(top_srcdir)/src/libproton.la

getopt_SOURCES = getopt.cpp
getopt_CXXFLAGS = $(BOOST_CPPFLAGS)
getopt_LDADD = $(top_srcdir)/src/libproton.la

INCLUDES = -I$(top_srcdir)/include
