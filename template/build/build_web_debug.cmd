@echo off

call %EMSDK%/emsdk_env.bat

IF NOT EXIST _build_web_debug\ (
  mkdir _build_web_debug
  cd _build_web_debug
  emcmake cmake -DYOURGAME_PLATFORM=web -DCMAKE_BUILD_TYPE=DEBUG ../..
  emmake cmake --build .
  cd ..
) ELSE (
  cd _build_web_debug
  emmake cmake --build .
  cd ..
)
