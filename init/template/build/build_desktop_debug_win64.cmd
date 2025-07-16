@echo off
setlocal

set BUILD_DIR=_build_debug_win64

IF NOT EXIST %BUILD_DIR% (
  mkdir %BUILD_DIR%
  cmake -A x64 -B %BUILD_DIR% -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=Debug ..
)

cmake --build %BUILD_DIR%

endlocal
