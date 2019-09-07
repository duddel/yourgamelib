# YourGameLib

**Framework for game-like applications - based on CMake, C++, OpenGL (ES)**

-   Kick-start cross-platform OpenGL projects written in C/C++
-   Automated integration of common third-party libraries

## License

The files in this repository are licensed under the zlib license (if not stated otherwise), except for files in subdirectories of thirdparty/. For license information of thirdparty libraries, see: [thirdparty/](thirdparty/).

## Usage

Download or clone this repository and add the root directory to your CMake project via `add_subdirectory()`, like so:

```cmake
# omit if no further dependencies required
list(APPEND YOURGAME_EXT_PROJ_LIST
  "imgui"
)

# omit if multiple platforms are supported
set(YOURGAME_PLATFORM desktop)

add_subdirectory(libyourgame)
```

This creates the CMake library target `yourgame` to link your code against.

If multiple platforms are supported, omit setting `YOURGAME_PLATFORM` in your CMakeLists.txt and hand over the platform via command line argument for each invokation of CMake:

```bash
cmake .. -DYOURGAME_PLATFORM=desktop
```

Check out the example: [examples/mini-gl](examples/mini-gl).

## Platforms

Supported platform to date:

    YOURGAME_PLATFORM=desktop
