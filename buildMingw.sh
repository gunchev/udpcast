#!/bin/sh

# Build Mingw (Windows) executable
#
# For this you meed the mingw cross-compiler to be installed
#
# You may download the RPMs from http://mirzam.it.vu.nl/mingw/
# All 4 RPM's are needed:
#  mingw-binutils
#  mingw-gcc-core
#  mingw-runtime
#  mingw-w32api

exec make LIBS="-lws2_32 -liphlpapi" EXESUFFIX=.exe CC=i386-mingw32-gcc "$@"
