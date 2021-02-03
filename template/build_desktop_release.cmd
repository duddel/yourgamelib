@echo off

IF NOT EXIST _build_release\ (
  mkdir _build_release
  cd _build_release
  cmake -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=RELEASE -DCPACK_GENERATOR="ZIP" ..
  cd ..
)

cd _build_release && cmake --build . --target package  && cd ..
