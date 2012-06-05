#!/bin/sh
if test ! -d config; then
    mkdir config
fi
aclocal
autoconf
libtoolize
automake --add-missing
./configure
