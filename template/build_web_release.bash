#!/bin/bash

source $EMSDK/emsdk_env.sh

if [ ! -d "_build_web_release" ]; then
  mkdir _build_web_release
  cd _build_web_release
  emcmake cmake -DYOURGAME_PLATFORM=web -DCMAKE_BUILD_TYPE=RELEASE -DCPACK_GENERATOR="TGZ;ZIP" ..
  cd -
fi

cd _build_web_release
emmake cmake --build . --target package
cd -

if [ ! -d "_deploy" ]; then
  mkdir _deploy
fi

mv -f _build_web_release/*.tar.gz _deploy/
mv -f _build_web_release/*.zip _deploy/
cd -
