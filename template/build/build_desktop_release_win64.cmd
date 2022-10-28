@echo off

IF NOT EXIST _build_release_win64\ (
  mkdir _build_release_win64
  cd _build_release_win64
  cmake -A x64 -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=RELEASE -DCPACK_GENERATOR="ZIP" ../..
  cd ..
)

REM --config Release is required for VS solutions to actually build the
REM Release configuration (default Debug). -DCMAKE_BUILD_TYPE=RELEASE
REM at configure time has no effect on the VS solution.
cd _build_release_win64 && cmake --build . --target package --config Release && cd ..

IF NOT EXIST _deploy\ (
  mkdir _deploy
)

move /Y _build_release_win64\*.zip _deploy
