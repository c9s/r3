#!/bin/sh

apt-get update -qq
apt-get install -qq \
    autoconf \
    automake \
    build-essential \
    check \
    clang \
    cmake \
    graphviz-dev \
    libjemalloc-dev \
    libpcre3-dev \
    libtool \
    ninja-build \
    pkg-config

if [ x$COVERALLS == xyes ]; then
    pip install cpp-coveralls
fi

if [ x$VALGRIND == xyes ]; then
    apt-get install valgrind
fi
