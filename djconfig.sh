#!/bin/sh
export CFLAGS="-O3 -funroll-loops -march=pentiumpro -mpentiumpro -malign-functions=4 -malign-loops=4"
./configure $*
