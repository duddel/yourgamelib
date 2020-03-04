@echo off

IF NOT EXIST _build_release\ (
  mkdir _build_release
  cd _build_release
  cmake -DYOURGAME_PLATFORM=desktop -DCPACK_GENERATOR="ZIP" -DCMAKE_BUILD_TYPE=RELEASE ..
  cd ..
)

cd _build_release && cmake --build . --target package && cd ..
