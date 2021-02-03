#!/bin/bash

if [ ! -d "_build" ]; then
  mkdir _build
  cd _build
  cmake -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=DEBUG ..
  cd -
fi

cd _build
cmake --build .
cd -
