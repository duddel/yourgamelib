#!/bin/bash

source $EMSDK/emsdk_env.sh

if [ ! -d "_build_web" ]; then
  mkdir _build_web
  cd _build_web
  emcmake cmake .. -DYOURGAME_PLATFORM=web
  cd -
fi

cd _build_web
emmake cmake --build .
cd -
