lib_LTLIBRARIES = libproton.la

libproton_la_SOURCES = base.cpp pool.cpp
libproton_la_CXXFLAGS = $(BOOST_CPPFLAGS)
libproton_la_LDFLAGS = -version-info 2:0:0 -release 1.1.1 -no-undefined

proton_includedir=$(includedir)/proton/
proton_include_HEADERS=$(top_srcdir)/include/proton/*.hpp
protondetail_includedir=$(includedir)/proton/detail/
protondetail_include_HEADERS=$(top_srcdir)/include/proton/detail/*.hpp

INCLUDES = -I$(top_srcdir)/include
