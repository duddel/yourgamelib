#!/bin/bash

source $EMSDK/emsdk_env.sh

if [ ! -d "_build_web_release" ]; then
  mkdir _build_web_release
  cd _build_web_release
  emcmake cmake -DYOURGAME_PLATFORM=web -DCPACK_GENERATOR="TGZ,ZIP" ..
  cd -
fi

cd _build_web_release
emmake cmake --build . --target package
cd -
