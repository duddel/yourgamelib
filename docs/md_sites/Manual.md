# Manual

> **NOTE:** This site is UNDER CONSTRUCTION!

## Init Script

To integrate additional [third-party libraries](thirdparty/README.md), use the `--extProj` option, like so:

```bash
python <path_to_yourgamelib>/init/init.py MyGame --extProj box2d lua
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

The Android project is configured to be built with a separate Gradle installation (Tested with 8.0.1). To build it, you need to

-   Install the Android SDK
-   set the environment variable `ANDROID_SDK_HOME` pointing to your Android SDK installation

If this stops working with a newer Gradle version, open the project in Android Studio and let it generate a Gradle Wrapper for the project.

Or try to fix it by looking into [build.gradle](template/build/android/app/build.gradle). Maybe some dependencies have to be installed manually via the `sdkmanager`, or have to be updated in `build.gradle` to work with a newer Gradle version.

## WebAssembly

### Requirements

To build for target `web`, you need:

-   Emscripten
-   Make sure the environment variable `EMSDK` is set and points to the emsdk root directory.
