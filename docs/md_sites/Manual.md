# Manual

> **NOTE:** This site is UNDER CONSTRUCTION!

## Init Script

To integrate additional [third-party libraries](thirdparty/README.md), use the `--extProj` option, like so:

```bash
python <path_to_yourgamelib>/init.py MyGame --extProj box2d lua
```

## Build System

`yourgamelib` is pulled into `MyGame` via CMake `add_subdirectory()`. This path needs to be adjusted in `CMakeLists.txt` if the projects are moved around. Alternatively, `MyGame` can be configured to clone `yourgamelib` by itself to make it independent of the original `yourgamelib` location. See [template/CMakeLists.txt](template/CMakeLists.txt) on how to do this.

## Assets (Save files, Project files)

All files placed in `assets/` will be packed and made available to be loaded by the application at runtime.

## User Code

## YourGame API and Namespaces

## Toolbox

## Third-party libraries

## Android

### Requirements

To build for target `android`, you need:

-   The Android SDK
    -   check yourgame template [build.gradle](template/android/app/build.gradle)...
        -   ...and install the requested **NDK** version (`ndkVersion`) via **sdkmanager**, or change suitable
        -   ...do the same for other Android SDK components, if they are not installed automatically
-   Gradle (tested version 6.8.3), **or...**
-   ...use Android Studio. It generates a Gradle Wrapper for the project, not requiring a separate Gradle installation

## WebAssembly

### Requirements

To build for target `web`, you need:

-   Emscripten
-   Make sure the environment variable `EMSDK` is set and points to the emsdk root directory.
