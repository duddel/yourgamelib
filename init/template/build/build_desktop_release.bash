#!/bin/bash

if [ ! -d "_build_release" ]; then
  mkdir _build_release
  cd _build_release
  cmake -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=RELEASE -DCPACK_GENERATOR="TGZ" ../.. || exit $?
  cd -
fi

cd _build_release
cmake --build . --target package || exit $?
cd -

if [ ! -d "_deploy" ]; then
  mkdir _deploy
fi

mv -f _build_release/*.tar.gz _deploy/ || exit $?
