@echo off

IF NOT EXIST _deploy\ (
  mkdir _deploy
)

call %EMSDK%/emsdk_env.bat

IF NOT EXIST _build_web_release\ (
  mkdir _build_web_release
  cd _build_web_release
  emcmake cmake -DYOURGAME_PLATFORM=web -DCPACK_GENERATOR="ZIP" ..
  emmake cmake --build . --target package
  move /Y *.zip ..\_deploy
  cd ..
) ELSE (
  cd _build_web_release
  emmake cmake --build . --target package
  move /Y *.zip ..\_deploy
  cd ..
)
