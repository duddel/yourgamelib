#!/bin/bash

if [ ! -d "_build_release" ]; then
  mkdir _build_release
  cd _build_release
  cmake -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=RELEASE -DCPACK_GENERATOR="TGZ" ..
  cd -
fi

cd _build_release
cmake --build . --target package
cd -
