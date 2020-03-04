#!/bin/bash

if [ ! -d "_build_release" ]; then
  mkdir _build_release
  cd _build_release
  cmake -DYOURGAME_PLATFORM=desktop -DCPACK_GENERATOR="TGZ" -DCMAKE_BUILD_TYPE=RELEASE ..
  cd -
fi

cd _build_release
cmake --build . --target package
cd -
