#!/bin/bash

source $EMSDK/emsdk_env.sh

if [ ! -d "_build_web_release" ]; then
  mkdir _build_web_release
  cd _build_web_release
  emcmake cmake -DYOURGAME_PLATFORM=web -DCMAKE_BUILD_TYPE=RELEASE -DCPACK_GENERATOR="TGZ;ZIP" ../.. || exit $?
  cd -
fi

cd _build_web_release
emmake cmake --build . --target package || exit $?
cd -

if [ ! -d "_deploy" ]; then
  mkdir _deploy
fi

mv -f _build_web_release/*.tar.gz _deploy/ || exit $?
mv -f _build_web_release/*.zip _deploy/ || exit $?
cd -
