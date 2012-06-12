#!/bin/sh
if test ! -d config; then
    mkdir config
fi
if test ! -f configure; then
    aclocal
    autoconf
    libtoolize
    automake --add-missing
fi
autoreconf
