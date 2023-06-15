@echo off

IF NOT EXIST _build_win64\ (
  mkdir _build_win64
  cd _build_win64
  cmake -A x64 -DCMAKE_BUILD_TYPE=DEBUG ..
  cd ..
)

cd _build_win64 && cmake --build .
ctest -C Debug -V
cd ..
