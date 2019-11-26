# YourGameLib

[![Examples](https://github.com/duddel/yourgamelib/workflows/examples/badge.svg)](https://github.com/duddel/yourgamelib/actions?query=workflow%3Aexamples)

**Framework for game-like applications - based on CMake, C++, OpenGL (ES)**

-   Kick-start cross-platform OpenGL projects written in C/C++
-   Automated integration of common third-party libraries

## License

-   The source code in this repository (except for code in subdirectories of thirdparty/) is licensed under the zlib license (if not stated otherwise)
-   For license information of thirdparty libraries, see: [thirdparty/](thirdparty/)

## Usage

-   Download or clone this repository and add the root directory to your CMake project via `add_subdirectory()`
-   This creates the CMake library target `yourgame` to link your code against
-   Check out the [framework](examples/framework) example

## Platforms

Supported platform to date:

```bash
YOURGAME_PLATFORM=desktop
YOURGAME_PLATFORM=wasm
YOURGAME_PLATFORM=android
```

### Raspberry PI

Tested on Raspbian stretch, Pi 2

```bash
sudo apt install git cmake build-essential libgles2-mesa-dev xorg-dev
-DYOURGAME_PLATFORM=desktop -DYOURGAME_FORCE_GLES=ON
```

### Android

Make sure the environment variable `ANDROID_SDK_ROOT` is set and points to the android sdk. Install `ndk-bundle` and `cmake` via the `sdkmanager` (not installed platforms and platform-tools will be installed on demand)

tested with

```bash
cmake       version 3.6.4111459
ndk-bundle  version 20.0.5594570
```

Try the `framework` example:

```bash
cd examples/framework/android
./gradlew assembleDebug
```
