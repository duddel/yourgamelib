@echo off

IF NOT EXIST _build\ (
  mkdir _build
  cd _build
  cmake -DYOURGAME_PLATFORM=desktop ..
  cd ..
) ELSE (
  cd _build && cmake --build . && cd ..
)
