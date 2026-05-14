#!/bin/bash
docker run -e CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=RelWithDebInfo" -v ./:/redline:rw -v ./build:/build:rw redline-decomp
