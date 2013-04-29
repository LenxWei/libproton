#!/bin/sh
if test ! -d config; then
    mkdir config
fi
if test ! -f configure; then
    aclocal
    autoconf
    if type libtoolize; then
       libtoolize
    else
       if type glibtoolize; then
          glibtoolize
       else
          rm -rf config configure
          echo "please install libtoolize (glibtoolize on OSX)"
       fi
    fi
    automake --add-missing
fi
autoreconf
