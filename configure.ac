#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.68)
AC_INIT(proton, 1.1.0, lenx.wei@gmail.com)
AC_CONFIG_SRCDIR([src/base.cpp])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
# AC_CONFIG_HEADER([include/proton/config.h])

AM_INIT_AUTOMAKE(libproton.la, 1.1.0)

# Checks for programs.
AC_LANG([C++])
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext])
AX_CHECK_COMPILE_FLAG([-Wall -Wextra -Wno-unused-variable -Wno-unused-but-set-variable \
    -Wno-unused-parameter],\
    [CXXFLAGS="$CXXFLAGS -Wall -Wextra -Wno-unused-variable -Wno-unused-but-set-variable \
    -Wno-unused-parameter"])
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([ctime cstring cstdio])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 test/Makefile])
AC_OUTPUT
