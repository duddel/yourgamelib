# YourGameLib - Thirdparty

Here is a list of third-party libraries ("External Projects") that can optionally be integrated, or are mandatory for specific target platforms. Below is a detailed list of the origins, licenses and possible modifications for each library. Also, each library has a `ExtProj-<alias>.cmake` file (in [cmake/](../cmake)) that can be investigated for details about the integration.

## Mandatory dependencies

| Alias                                     | Name               | Desktop                       | Android            | Web                |
| ----------------------------------------- | ------------------ | ----------------------------- | ------------------ | ------------------ |
| [android-native-app](#android-native-app) | Android Native App |                               | :heavy_check_mark: |                    |
| [dirent](#dirent)                         | Dirent             | :heavy_check_mark: (Win only) |                    |                    |
| [easyloggingpp](#easylogging)             | Easylogging++      | :heavy_check_mark:            | :heavy_check_mark: | :heavy_check_mark: |
| [glad](#glad)                             | glad               | :heavy_check_mark:            |                    |                    |
| [glfw](#glfw)                             | GLFW               | :heavy_check_mark:            |                    |                    |
| [whereami](#where-am-i)                   | Where Am I?        | :heavy_check_mark:            |                    |                    |

## Toolbox libraries

These libraries are integrated if the project has been initialized with default settings, which include the _toolbox_ (by **not** using the `--bare` option). See [cmake/Toolbox.cmake](../cmake/Toolbox.cmake) for details.

| Alias                           | Name                | Desktop            | Android            | Web                |
| ------------------------------- | ------------------- | ------------------ | ------------------ | ------------------ |
| [glm](#glm)                     | glm                 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [imgui](#dear-imgui)            | Dear Imgui          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [json](#json-for-modern-c++)    | JSON for Modern C++ | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [stb](#stb)                     | stb                 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [tinyobjloader](#tinyobjloader) | tinyobjloader       | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |

## Optional libraries

| Alias                                  | Name                  | Desktop            | Android            | Web                |
| -------------------------------------- | --------------------- | ------------------ | ------------------ | ------------------ |
| [box2d](#box2d)                        | Box2D                 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [bullet](#bullet)                      | Bullet                | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [doctest](#doctest)                    | doctest               | :heavy_check_mark: |                    |                    |
| [file-dialogs](#portable-file-dialogs) | Portable File Dialogs | :heavy_check_mark: |                    |                    |
| [flecs](#flecs)                        | Flecs                 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [fsm](#fsm)                            | fsm                   | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [lua](#lua)                            | Lua, LuaBridge        | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| [miniaudio](#miniaudio)                | miniaudio             | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |

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

## Bullet

-   version: **2.89**
-   origin: <https://github.com/bulletphysics/bullet3/releases/tag/2.89>
-   extracted from origin:
    -   LICENSE.txt
    -   src/Bullet3Common/\*
    -   src/BulletCollision/\*
    -   src/BulletDynamics/\*
    -   src/BulletInverseDynamics/\*
    -   src/BulletSoftBody/\*
    -   src/LinearMath/\*
    -   src/btBulletCollisionCommon.h
    -   src/btBulletDynamicsCommon.h
    -   src/btLinearMathAll.cpp
-   license: [bullet/LICENSE.txt](bullet/LICENSE.txt)

## Dirent

-   version: **1.23.2**
-   origin: <https://github.com/tronkko/dirent/releases/tag/1.23.2>
-   extracted from origin:
    -   include/dirent.h, LICENSE
-   license: [dirent/LICENSE](dirent/LICENSE)

## doctest

-   version: **2.4.8**
-   origin: <https://github.com/doctest/doctest/releases/tag/v2.4.8>
-   extracted from origin:
    -   doctest.h, LICENSE.txt
-   license: [doctest/LICENSE.txt](doctest/LICENSE.txt)

## Easylogging++

-   version: **9.96.7**
-   origin: <https://github.com/zuhd-org/easyloggingpp/releases/tag/v9.96.7>
-   extracted from origin:
    -   LICENSE, easylogging++.{h,cc}
-   license: [easyloggingpp/LICENSE](easyloggingpp/LICENSE)

## Portable File Dialogs

-   version: **7a7a9f5 (master)**
-   origin: <https://github.com/samhocevar/portable-file-dialogs/tree/7a7a9f5fa800b28331672742c0e774d46186e34f>
-   extracted from origin:
    -   COPYING, portable-file-dialogs.h
-   license: [portable-file-dialogs/COPYING](portable-file-dialogs/COPYING)

## Flecs

-   version: **2.3.2**
-   origin: <https://github.com/SanderMertens/flecs/releases/tag/v2.3.2>
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

-   version: **1.82**
-   origin: <https://github.com/ocornut/imgui/releases/tag/v1.82>
-   extracted from origin: see files below
-   license: [imgui/LICENSE.txt](imgui/LICENSE.txt)
    -   backends/imgui_impl_android.{h,cpp}
    -   backends/imgui_impl_glfw.{h,cpp}
    -   backends/imgui_impl_opengl3.{h,cpp}
    -   imconfig.h
    -   imgui.{h,cpp}
    -   imgui_demo.cpp
    -   imgui_draw.cpp
    -   imgui_internal.h
    -   imgui_tables.cpp
    -   imgui_widgets.cpp
-   license: mentioned in files
    -   [imstb_rectpack.h](imgui/imstb_rectpack.h)
    -   [imstb_textedit.h](imgui/imstb_textedit.h)
    -   [imstb_truetype.h](imgui/imstb_truetype.h)

### ImGuiColorTextEdit

-   version: **0a88824f7d**
-   origin: <https://github.com/BalazsJako/ImGuiColorTextEdit/tree/0a88824f7de8d0bd11d8419066caa7d3469395c4>
-   extracted from origin:
    -   TextEditor.{h,cpp}, LICENSE
-   license: [imgui/TextEditor_LICENSE](imgui/TextEditor_LICENSE)

### imgui_memory_editor

-   version: **450b9cf352**
-   origin: <https://github.com/ocornut/imgui_club/tree/450b9cf352c51530b9e23fb14b83891c2555b032>
-   extracted from origin:
    -   imgui_memory_editor.h, LICENSE.txt
-   license: [imgui/imgui_memory_editor_LICENSE.txt](imgui/imgui_memory_editor_LICENSE.txt)

## JSON for Modern C++

-   version: **3.8.0**
-   origin: <https://github.com/nlohmann/json/releases/tag/v3.8.0>
-   extracted from origin:
    -   json.hpp, LICENSE.MIT
-   license: [json/LICENSE.MIT](json/LICENSE.MIT)

## Lua, LuaBridge

### Lua

-   version: **5.4.0**
-   origin: <https://github.com/lua/lua/releases/tag/v5.4.0>
-   extracted from origin:
    -   all .h and .c files from repo root
-   license: see end of file [lua/lua.h](lua/lua.h)

### LuaBridge

-   version: **2.7, fab7b33 (master)**
-   origin: <https://github.com/vinniefalco/LuaBridge/tree/fab7b33b896a42dcc865ba5ecdbacd9f409137f8>
-   extracted from origin:
    -   Source/LuaBridge/\*
-   license: [luabridge/license.txt](luabridge/license.txt)

## miniaudio

-   version: **0.10.16 (unreleased), 54fe965 (dev)**
-   origin: <https://github.com/dr-soft/miniaudio/tree/54fe965bc019ebc6e96a0c04a0a5550c1ca8b2fe>
-   extracted from origin:
    -   miniaudio.h, LICENSE
-   license: [miniaudio/LICENSE](miniaudio/LICENSE)

## stb

-   version: **c9064e3 (master)**
-   origin: <https://github.com/nothings/stb/tree/c9064e317699d2e495f36ba4f9ac037e88ee371a>
-   extracted from origin: see files below
-   license: mentioned in files
    -   [stb_image.h](stb/stb_image.h), version: **2.26**
    -   [stb_image_write.h](stb/stb_image_write.h), version: **1.15**
    -   [stb_vorbis.c](stb/stb_vorbis.c), version: **1.20**

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
