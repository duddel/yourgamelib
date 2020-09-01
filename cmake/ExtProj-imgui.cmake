#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/examples
)
list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/imgui.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/imgui_demo.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/imgui_draw.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/imgui_widgets.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/examples/imgui_impl_opengl3.cpp
)
") # file()

# platform specific
if(YOURGAME_PLATFORM STREQUAL "android")
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/examples/imgui_impl_android.cpp
)
") # file()
else()
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/examples/imgui_impl_glfw.cpp
)
") # file()
endif()
