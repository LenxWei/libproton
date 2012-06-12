TESTS = base_test pool_ut ref_ut stl_test
check_PROGRAMS = base_test pool_ut ref_ut stl_test

base_test_SOURCES = base_test.cpp
base_test_CXXFLAGS = $(BOOST_CPPFLAGS)
base_test_LDADD = $(top_srcdir)/src/libproton.la

pool_ut_SOURCES = pool_ut.cpp
pool_ut_CXXFLAGS = $(BOOST_CPPFLAGS)
pool_ut_LDADD = $(top_srcdir)/src/libproton.la

ref_ut_SOURCES = ref_ut.cpp
ref_ut_CXXFLAGS = $(BOOST_CPPFLAGS)
ref_ut_LDADD = $(top_srcdir)/src/libproton.la

stl_test_SOURCES = test.cpp
stl_test_CXXFLAGS = $(BOOST_CPPFLAGS)
stl_test_LDADD = $(top_srcdir)/src/libproton.la

INCLUDES = -I$(top_srcdir)/include
