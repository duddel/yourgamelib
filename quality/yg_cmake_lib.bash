#!/bin/bash

# Create empty _build/
mkdir -p _build
rm -rf _build/*

# Perform CMake configure
cd _build
cmake -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../.. || exit $?
cd -
