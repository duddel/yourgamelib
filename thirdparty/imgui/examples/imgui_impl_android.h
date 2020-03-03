// dear imgui: Platform Binding for Android native app
// This needs to be used along with the OpenGL 3 Renderer (imgui_impl_opengl3)

// Implemented features:
//  [x] Basic mouse input via touch
//  [ ] open soft keyboard if io.WantTextInput and perform proper keyboard input

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#pragma once

struct android_app;
struct AInputEvent;

IMGUI_IMPL_API bool     ImGui_ImplAndroid_Init(struct android_app *app, int32_t (*userOnInputEvent)(struct android_app *app, AInputEvent *event) = NULL);
IMGUI_IMPL_API void     ImGui_ImplAndroid_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplAndroid_NewFrame();
