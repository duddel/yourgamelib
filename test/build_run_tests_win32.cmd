@echo off

IF NOT EXIST _build_win32\ (
  mkdir _build_win32
  cd _build_win32
  cmake -A Win32 -DCMAKE_BUILD_TYPE=DEBUG ..
  cd ..
)

cd _build_win32 && cmake --build .
ctest -C Debug -V
cd ..
