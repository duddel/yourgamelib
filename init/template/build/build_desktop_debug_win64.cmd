@echo off

IF NOT EXIST _build_debug_win64\ (
  mkdir _build_debug_win64
  cd _build_debug_win64
  cmake -A x64 -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=DEBUG ../..
  cd ..
)

cd _build_debug_win64 && cmake --build . && cd ..
