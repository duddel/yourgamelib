#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/imgui
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/imgui/examples
)
list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/imgui/imgui.cpp
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/imgui/imgui_demo.cpp
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/imgui/imgui_draw.cpp
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/imgui/imgui_widgets.cpp
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/imgui/examples/imgui_impl_opengl3.cpp
)
") # file()

# platform specific
if(YOURGAME_PLATFORM STREQUAL "android")
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/imgui/examples/imgui_impl_android.cpp
)
list(APPEND YOURGAME_COMPILE_DEFS
  IMGUI_IMPL_OPENGL_LOADER_CUSTOM
)
") # file()
else()
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/imgui/examples/imgui_impl_glfw.cpp
)
list(APPEND YOURGAME_COMPILE_DEFS
  IMGUI_IMPL_OPENGL_LOADER_GLAD
)
") # file()
endif()
