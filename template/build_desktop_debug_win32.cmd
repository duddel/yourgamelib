@echo off

IF NOT EXIST _build_debug_win32\ (
  mkdir _build_debug_win32
  cd _build_debug_win32
  cmake -A Win32 -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=DEBUG ..
  cd ..
)

cd _build_debug_win32 && cmake --build . && cd ..
