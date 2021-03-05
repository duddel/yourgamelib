@echo off

IF NOT EXIST _build_release\ (
  mkdir _build_release
  cd _build_release
  cmake -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=RELEASE -DCPACK_GENERATOR="ZIP" ..
  cd ..
)

REM --config Release is required for VS solutions to actually build the
REM Release configuration (default Debug). -DCMAKE_BUILD_TYPE=RELEASE
REM at configure time has no effect on the VS solution.
cd _build_release && cmake --build . --target package --config Release && cd ..

IF NOT EXIST _deploy\ (
  mkdir _deploy
)

move /Y _build_release\*.zip _deploy
