@echo off

call %EMSDK%/emsdk_env.bat

IF NOT EXIST _build_web\ (
  mkdir _build_web
  cd _build_web
  emconfigure cmake -DYOURGAME_PLATFORM=web ..
  emmake cmake --build .
  cd ..
) ELSE (
  cd _build_web
  emmake cmake --build .
  cd ..
)
