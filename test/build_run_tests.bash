#!/bin/bash

if [ ! -d "_build" ]; then
  mkdir _build
  cd _build
  cmake -DCMAKE_BUILD_TYPE=DEBUG .. || exit $?
  cd -
fi

cd _build
cmake --build . || exit $?
ctest -V || exit $?
cd -
