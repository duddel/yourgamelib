# YourGameLib

[![Examples](https://github.com/duddel/yourgamelib/workflows/examples/badge.svg)](https://github.com/duddel/yourgamelib/actions?query=workflow%3Aexamples)
[![Docs](https://github.com/duddel/yourgamelib/workflows/docs/badge.svg)](https://github.com/duddel/yourgamelib/actions?query=workflow%3Adocs)

**Framework for game-like applications - based on CMake, C++, OpenGL (ES)**

-   Kick-start cross-platform OpenGL projects written in C/C++
-   Automated integration of common third-party libraries

## Kick-start

Download or clone this repository and run the Python script `tools/init.py` from an arbitrary location, like so:

```bash
python <path_to_yourgamelib>/tools/init.py MyGame
```

To integrate additional third-party libraries, use the `--extProj` option, like so:

```bash
python <path_to_yourgamelib>/tools/init.py MyGame --extProj imgui glm
```

This creates the directory `./MyGame` and initializes a new project called `MyGame` with this content:

    android/
    assets/
    build_android.bash
    build_android.cmd
    build_desktop.bash
    build_desktop.cmd
    build_web.bash
    build_web.cmd
    CMakeLists.txt
    mygame.cpp

The minimum example can be build for all available platforms using the `build_[..]` scripts (see requirements below). See the [API documentation](https://duddel.github.io/yourgamelib/) and the [examples/](examples/) on how to create something ontop of the template.

All files placed in `assets/` will be packed and made available to be loaded by the application at runtime.

> **NOTE:** `yourgamelib` is pulled into `MyGame` via CMake `add_subdirectory()`. This path needs to be adjusted in `CMakeLists.txt` if the projects are moved around.

## Requirements

To initialize a new project, you need:

-   `Python 3.5+` with `Jinja2`

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
