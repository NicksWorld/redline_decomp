#!/usr/bin/env bash

wine reg ADD 'HKCU\Environment' /v PATH /d 'C:\msvc\vc98\bin;C:\msvc\common\msdev98\bin;C:\cmake\bin;C:\windows\system32' /f
wine reg ADD 'HKCU\Environment' /v INCLUDE /d 'C:\msvc\vc98\include;C:\msvc\vc98\mfc\include' /f
wine reg ADD 'HKCU\Environment' /v LIB /d 'C:\msvc\vc98\lib' /f
wine reg ADD 'HKCU\Environment' /v TMP /d 'Z:\build' /f
wine reg ADD 'HKCU\Environment' /v TEMP /d 'Z:\build' /f

# Configure build with CMake
wine cmake -B build redline -G "NMake Makefiles" $CMAKE_FLAGS

# Start compiling LEGO Island
wine cmake --build build

# Unlock directories
chmod -R 777 build
