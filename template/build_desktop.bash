#!/bin/bash

if [ ! -d "_build" ]; then
  mkdir _build
  cd _build
  cmake -DYOURGAME_PLATFORM=desktop ..
  cd -
fi

cd _build
cmake --build .
cd -
