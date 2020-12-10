# YourGameLib - Thirdparty

Here is a list of third-party libraries ("External Projects") that can optionally be integrated, or are mandatory for specific target platforms. Below is a detailed list of the origins, licenses and possible modifications for each library. Also, each library has a `ExtProj-<alias>.cmake` file (in [cmake/](../cmake)) that can be investigated for details about the integration.

## Optional libraries

| Alias                           | Name                | Desktop            | Android            | Web                |
| ------------------------------- | ------------------- | ------------------ | ------------------ | ------------------ |
| [box2d](#box2d)                 | Box2D               | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [choreograph](#choreograph)     | Choreograph         | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [flecs](#flecs)                 | Flecs               | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [fsm](#fsm)                     | fsm                 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [glm](#glm)                     | glm                 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [imgui](#dear-imgui)            | Dear Imgui          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [json](#json-for-modern-c++)    | JSON for Modern C++ | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [lua](#lua)                     | Lua                 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [miniaudio](#miniaudio)         | miniaudio           | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [qu3e](#qu3e)                   | qu3e                | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [stb](#stb)                     | stb                 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [tinyobjloader](#tinyobjloader) | tinyobjloader       | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |

## Mandatory dependencies

| Alias                                     | Name               | Desktop            | Android            | Web                |
| ----------------------------------------- | ------------------ | ------------------ | ------------------ | ------------------ |
| [android-native-app](#android-native-app) | Android Native App |                    | :heavy_check_mark: |                    |
| [easyloggingpp](#easylogging)             | Easylogging++      | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [glad](#glad)                             | glad               | :heavy_check_mark: |                    |                    |
| [glfw](#glfw)                             | GLFW               | :heavy_check_mark: |                    |                    |
| [whereami](#where-am-i)                   | Where Am I?        | :heavy_check_mark: |                    |                    |

* * *

## Android Native App

-   code is used from the Android SDK installation (no code distributed)
-   see [ExtProj-android-native-app.cmake](../cmake/ExtProj-android-native-app.cmake)
-   license: check license information included in Android SDK

## Box2D

-   version: **4de0ea3, 2.4.0 (unreleased)**
-   origin: <https://github.com/erincatto/box2d/tree/4de0ea3f1e9511ebd4c85b52e200a8bd9c2fae9e>
-   extracted from origin:
    -   LICENSE
    -   include/\*
    -   src/collision/\*
    -   src/common/\*
    -   src/dynamics/\*
    -   src/rope/\*
-   modifications are marked with `YOURGAME_MODIFICATION`
-   license: [box2d/LICENSE](box2d/LICENSE)

## Choreograph

-   version: **3463198, 0.4.0**
-   origin: <https://github.com/sansumbrella/Choreograph/tree/34631980702f5d3745ac29d262712dd81b594ade>
-   extracted from origin:
    -   LICENSE.md
    -   src/\*
-   modifications are marked with `YOURGAME_MODIFICATION`
-   license: [choreograph/LICENSE.md](choreograph/LICENSE.md)

## Easylogging++

-   version: **9.96.7**
-   origin: <https://github.com/zuhd-org/easyloggingpp/releases/tag/v9.96.7>
-   extracted from origin:
    -   LICENSE, easylogging++.{h,cc}
-   license: [easyloggingpp/LICENSE](easyloggingpp/LICENSE)

## Flecs

-   version: **76a0221, 2.2.0**
-   origin: <https://github.com/SanderMertens/flecs/tree/76a02215982dee495494af7b651163eb234a48df>
-   extracted from origin:
    -   LICENSE, flecs.{h,c}
-   license: [flecs/LICENSE](flecs/LICENSE)

## fsm

-   version: **1.0.0**
-   origin: <https://github.com/r-lyeh-archived/fsm/tree/41c635ef95d838096f891579d254eccc163ee6af>
-   extracted from origin:
    -   LICENSE, fsm.hpp
-   license: [fsm/LICENSE](fsm/LICENSE)

## glad

-   version: **0.1.33 (via webtool)**
-   origin: see [glad.h](glad-gl3.3-core/include/glad/glad.h) 
-   license: _"Any of Public Domain, WTFPL or CC0."_, see: FAQ at <https://github.com/Dav1dde/glad>
    -   glad.{h,c}
-   license: mentioned in file
    -   [khrplatform.h](glad-gl3.3-core/include/KHR/khrplatform.h)

## GLFW

-   version: **3.3**
-   origin: <https://github.com/glfw/glfw/releases/tag/3.3>
-   extracted from origin:
    -   all, except deps/, docs/, examples/, tests/
-   license: [glfw/LICENSE.md](glfw/LICENSE.md)

## glm

-   version: **0.9.9.6**
-   origin: <https://github.com/g-truc/glm/releases/tag/0.9.9.6>
-   extracted from origin:
    -   glm/, copying.txt
-   license: [glm/copying.txt](glm/copying.txt)

## Dear ImGui

-   version: **1.79**
-   origin: <https://github.com/ocornut/imgui/releases/tag/v1.79>
-   extracted from origin: see files below
-   license: [imgui/LICENSE.txt](imgui/LICENSE.txt)
    -   examples/imgui_impl_glfw.{h,cpp}
    -   examples/imgui_impl_opengl3.{h,cpp}
    -   imconfig.h
    -   imgui.{h,cpp}
    -   imgui_demo.cpp
    -   imgui_draw.cpp
    -   imgui_internal.h
    -   imgui_widgets.cpp
-   license: mentioned in files
    -   [imstb_rectpack.h](imgui/imstb_rectpack.h)
    -   [imstb_textedit.h](imgui/imstb_textedit.h)
    -   [imstb_truetype.h](imgui/imstb_truetype.h)

### Dear ImGui Android back-end

-   version: **1297dda (android_native)**
-   origin: <https://github.com/duddel/imgui/tree/1297dda1e1ac016dd07ca4cbb521776169900aff>
-   extracted from origin: see files below
-   license: [imgui/LICENSE.txt](imgui/LICENSE.txt)
    -   backends/imgui_impl_android.{h,cpp}

## JSON for Modern C++

-   version: **3.8.0**
-   origin: <https://github.com/nlohmann/json/releases/tag/v3.8.0>
-   extracted from origin:
    -   json.hpp, LICENSE.MIT
-   license: [json/LICENSE.MIT](json/LICENSE.MIT)

## Lua

-   version: **5.4.0**
-   origin: <https://github.com/lua/lua/releases/tag/v5.4.0>
-   extracted from origin:
    -   all .h and .c files from repo root
-   license: see end of file [lua/lua.h](lua/lua.h)

## miniaudio

-   version: **0.10.16 (unreleased), 54fe965 (dev)**
-   origin: <https://github.com/dr-soft/miniaudio/tree/54fe965bc019ebc6e96a0c04a0a5550c1ca8b2fe>
-   extracted from origin:
    -   miniaudio.h, LICENSE
-   license: [miniaudio/LICENSE](miniaudio/LICENSE)

## qu3e

-   version: **1.01, ad3feda (master)**
-   origin: <https://github.com/RandyGaul/qu3e/tree/ad3feda2958f023f4989a69e5127265281cdf603>
-   extracted from origin
    -   src/\*
    -   CMakeLists.txt
    -   LICENSE.txt
-   license: [qu3e/LICENSE.txt](qu3e/LICENSE.txt)

## stb

-   version: **f54acd4 (master)**
-   origin: <https://github.com/nothings/stb/tree/f54acd4e13430c5122cab4ca657705c84aa61b08>
-   extracted from origin: see files below
-   license: mentioned in files
    -   [stb_image.h](stb/stb_image.h), version: **2.25**
    -   [stb_vorbis.c](stb/stb_vorbis.c), version: **1.19**

## tinyobjloader

-   version: **v2.0.0-rc1**
-   origin: <https://github.com/syoyo/tinyobjloader/releases/tag/v2.0.0-rc1>
-   extracted from origin:
    -   LICENSE, tiny_obj_loader.h
-   license: [tinyobjloader/LICENSE](tinyobjloader/LICENSE)

## Where Am I?

-   version: **f3a86fd (master)**
-   origin: <https://github.com/gpakosz/whereami/tree/f3a86fdf17b49c434a16bb4d9e45a135d4cc25f9>
-   extracted from origin:
    -   LICENSE.MIT, whereami.{h,c}
-   license: [whereami/LICENSE.MIT](whereami/LICENSE.MIT)
