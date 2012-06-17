TESTS = func vector
check_PROGRAMS = func vector

func_SOURCES = functor.cpp
func_CXXFLAGS = $(BOOST_CPPFLAGS)
func_LDADD = $(top_srcdir)/src/libproton.la

vector_SOURCES = vector.cpp
vector_CXXFLAGS = $(BOOST_CPPFLAGS)
vector_LDADD = $(top_srcdir)/src/libproton.la

INCLUDES = -I$(top_srcdir)/include
