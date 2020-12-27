# YourGameLib

[![Examples](https://github.com/duddel/yourgamelib/workflows/examples/badge.svg)](https://github.com/duddel/yourgamelib/actions?query=workflow%3Aexamples)
[![Docs](https://github.com/duddel/yourgamelib/workflows/docs/badge.svg)](https://github.com/duddel/yourgamelib/actions?query=workflow%3Adocs)
[![Tests](https://github.com/duddel/yourgamelib/workflows/tests/badge.svg)](https://github.com/duddel/yourgamelib/actions?query=workflow%3Atests)

**Framework for game-like applications - based on CMake, C++11, OpenGL (ES)**

-   Kick-start cross-platform OpenGL projects written in C++11
-   Automated integration of common third-party libraries

## Kick-start

Clone this repository and run the Python script `init.py` from an arbitrary location, like so:

    python <path_to_yourgamelib>/init.py MyGame

To integrate additional [third-party libraries](thirdparty/README.md), use the `--extProj` option, like so:

    python <path_to_yourgamelib>/init.py MyGame --extProj box2d lua

This creates the directory `./MyGame/` and initializes a new project called `MyGame`.

:zap: The minimum example can be build for all available platforms using the `build_[..]` scripts (see requirements below). See the [API documentation](https://duddel.github.io/yourgamelib/) and the [examples/](examples/) on how to create something ontop of the template.

:zap: All files placed in `assets/` will be packed and made available to be loaded by the application at runtime.

:zap: `yourgamelib` is pulled into `MyGame` via CMake `add_subdirectory()`. This path needs to be adjusted in `CMakeLists.txt` if the projects are moved around. Alternatively, `MyGame` can be configured to clone `yourgamelib` by itself to make it independent of the original `yourgamelib` location. See [template/CMakeLists.txt](template/CMakeLists.txt) on how to do this.

## Requirements

To initialize a new project, you need:

-   `Git 1.7.0+`

-   `Python 3.5+` with
    -   `Jinja2`
    -   `gitpython`

To build for target `desktop`, you need:

-   `CMake 3.6+`
-   A common C/C++ compiler capable of C++11

To build for target `android`, you need:

-   the Android SDK
    -   manually install `ndk-bundle` (tested version 20.0.5594570)
    -   and `cmake` (tested version 3.6.4111459) via the `sdkmanager`
-   Make sure the environment variable `ANDROID_SDK_ROOT` is set and points to the android sdk.

To build for target `web`, you need:

-   Emscripten
-   Make sure the environment variable `EMSDK` is set and points to the emsdk root directory.

## License

-   The source code in this repository (except for code in subdirectories of thirdparty/) is licensed under the zlib license (if not stated otherwise)
-   For license information of thirdparty libraries, see: [thirdparty/](thirdparty/)
