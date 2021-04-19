#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/backends
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/filedialog
)
list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/imgui.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/imgui_demo.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/imgui_draw.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/imgui_tables.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/imgui_widgets.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/filedialog/ImGuiFileDialog.cpp
)
") # file()

# on windows, imgui uses functions from imm.h, but imm32 library is not linked by mingw.
# ...or consider defining IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS in imconfig.h
if(YOURGAME_PLATFORM STREQUAL "desktop")
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") # todo: even if mingw is used, MSVC is reported
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_LIBS
  imm32
)
") # file()
endif()
endif()

# platform specific backends
if(YOURGAME_PLATFORM STREQUAL "android")
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/backends/imgui_impl_android.cpp
)
") # file()
else()
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/imgui/backends/imgui_impl_glfw.cpp
)
") # file()
endif()
