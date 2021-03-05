#!/bin/bash

if [ ! -d "_build_debug" ]; then
  mkdir _build_debug
  cd _build_debug
  cmake -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=DEBUG ..
  cd -
fi

cd _build_debug
cmake --build .
cd -
