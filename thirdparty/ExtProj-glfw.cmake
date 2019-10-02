#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"set(GLFW_BUILD_DOCS OFF CACHE BOOL \"glfw no docs\")
set(GLFW_BUILD_TESTS OFF CACHE BOOL \"glfw no tests\")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL \"glfw no examples\")
add_subdirectory(${YOURGAME_EXT_PROJ_MODULE_DIR}/glfw)
list(APPEND YOURGAME_LIBS
  glfw
)
list(APPEND YOURGAME_COMPILE_DEFS
  GLFW_INCLUDE_NONE
)
") # file()
