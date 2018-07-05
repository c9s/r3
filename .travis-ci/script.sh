#!/bin/sh

set -ev

./autogen.sh
./configure --enable-check $CONFIGURE_OPTION
make V=1
make install
if [ x$VALGRIND == xyes ]; then
    make check
else
    make check V=1
fi

# XXX: tracing memory leak, disabled for some mystery reason for automake...
#if [ x$VALGRIND == xyes && x$DEBUG == xyes ]; then
#    valgrind ./tests/check_* -v --trace-children=yes --show-leak-kinds=full --leak-check=full
#fi
