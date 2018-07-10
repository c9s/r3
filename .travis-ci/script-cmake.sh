#!/bin/sh

set -ev

mkdir build && cd build
cmake -GNinja ..
ninja -v
ctest
