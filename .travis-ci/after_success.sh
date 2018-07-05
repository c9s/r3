#!/bin/sh

if [ x$COVERALLS == xyes ]; then
    coveralls --exclude php --exclude 3rdparty
fi
