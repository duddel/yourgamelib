// dear imgui: Platform Binding for Android native app
// This needs to be used along with the OpenGL 3 Renderer (imgui_impl_opengl3)

// Implemented features:
//  [x] Basic mouse input via touch
//  [x] Open soft keyboard if io.WantTextInput and perform proper keyboard input
//  [x] Handle Unicode characters
//  [ ] Handle physical mouse input

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#pragma once

struct ANativeWindow;
struct AInputEvent;

IMGUI_IMPL_API int32_t  ImGui_ImplAndroid_handleInputEvent(AInputEvent *inputEvent);
IMGUI_IMPL_API bool     ImGui_ImplAndroid_Init(ANativeWindow *window);
IMGUI_IMPL_API void     ImGui_ImplAndroid_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplAndroid_NewFrame();
