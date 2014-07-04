#!/bin/bash

env CFLAGS="-O0 -g3" ./configure --with-blas --with-lapack --enable-R-static-lib --enable-maintainer-mode --with-valgrind-instrumentation=3
