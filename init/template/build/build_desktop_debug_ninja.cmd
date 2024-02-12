@echo off

IF NOT EXIST _build_debug_ninja\ (
  mkdir _build_debug_ninja
  cd _build_debug_ninja
  cmake -G "Ninja" -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=DEBUG ../..
  cd ..
)

cd _build_debug_ninja && cmake --build . && cd ..
