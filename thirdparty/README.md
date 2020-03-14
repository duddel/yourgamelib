# YourGameLib - Thirdparty

Here is a list of available third-party libraries ("External Projects") that can be integrated. Below is a detailed list of the origins, licenses and possible modifications for each library. Also, each library has a `ExtProj-<alias>.cmake` file that can be investigated for details about the integration.

-   _By User_ (**Yes**): these libraries are made available to the user if requested. They are **not** used internally by the `yourgame` library.
-   _By User_ (No): these libraries are integrated automatically depending on the target platform and used internally by the `yourgame` library. They should **not** be used in user code.

_Todo: This distinction could be made by public/private "linking", but this is not implemented yet._

| Alias                                     | Name               | By User |
| ----------------------------------------- | ------------------ | ------- |
| [glm](#glm)                               | glm                | **Yes** |
| [imgui](#dear-imgui)                      | Dear Imgui         | **Yes** |
| [stb](#stb)                               | stb                | **Yes** |
| [tinyobjloader](#tinyobjloader)           | tinyobjloader      | **Yes** |
| [android-native-app](#android-native-app) | Android Native App | No      |
| [easyloggingpp](#easylogging)             | Easylogging++      | No      |
| [glad](#glad)                             | glad               | No      |
| [glfw](#glfw)                             | GLFW               | No      |
| [whereami](#where-am-i)                   | Where Am I?        | No      |

* * *

## Android Native App

-   code is used from the Android SDK installation (no code distributed)
-   see [ExtProj-android-native-app.cmake](ExtProj-android-native-app.cmake)
-   license: check license information included in Android SDK

## Easylogging++

-   version: **9.96.7**
-   origin: <https://github.com/zuhd-org/easyloggingpp/releases/tag/v9.96.7>
-   extracted from origin:
    -   LICENSE, easylogging++.{h,cc}
-   license: [easyloggingpp/LICENSE](easyloggingpp/LICENSE)

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

-   version: **1.75**
-   origin: <https://github.com/ocornut/imgui/releases/tag/v1.75>
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

-   version: **0dff411 (android_native)**
-   origin: <https://github.com/duddel/imgui/tree/0dff411cd1b2779485d64c8a27957e8abdc0b3c3>
-   extracted from origin: see files below
-   license: [imgui/LICENSE.txt](imgui/LICENSE.txt)
    -   examples/imgui_impl_android.{h,cpp}

## miniaudio

-   version: **0.10.0, d1a166c (master)**
-   origin: <https://github.com/dr-soft/miniaudio/tree/d1a166c83ab445b1c14bc83d37c84e18d172e5f5>
-   extracted from origin:
    -   miniaudio.h, LICENSE
-   license: [miniaudio/LICENSE](miniaudio/LICENSE)

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
