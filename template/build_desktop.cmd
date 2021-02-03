@echo off

IF NOT EXIST _build\ (
  mkdir _build
  cd _build
  cmake -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=DEBUG ..
  cd ..
)

cd _build && cmake --build . && cd ..
