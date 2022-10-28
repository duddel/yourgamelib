#!/bin/bash

source $EMSDK/emsdk_env.sh

if [ ! -d "_build_web_debug" ]; then
  mkdir _build_web_debug
  cd _build_web_debug
  emcmake cmake -DYOURGAME_PLATFORM=web -DCMAKE_BUILD_TYPE=DEBUG ../..
  cd -
fi

cd _build_web_debug
emmake cmake --build .
cd -
