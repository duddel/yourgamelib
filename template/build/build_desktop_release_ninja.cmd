@echo off

IF NOT EXIST _build_release_ninja\ (
  mkdir _build_release_ninja
  cd _build_release_ninja
  cmake -G "Ninja" -DYOURGAME_PLATFORM=desktop -DCMAKE_BUILD_TYPE=RELEASE -DCPACK_GENERATOR="ZIP" ../..
  cd ..
)

cd _build_release_ninja && cmake --build . --target package && cd ..

IF NOT EXIST _deploy\ (
  mkdir _deploy
)

move /Y _build_release_ninja\*.zip _deploy
