set(GLFW_BUILD_DOCS OFF CACHE BOOL "glfw no docs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "glfw no tests")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "glfw no examples")
set(GLFW_INSTALL OFF CACHE BOOL "glfw no installation target")

add_subdirectory(${YOURGAME_EXT_PROJ_CODE_DIR}/glfw)

list(APPEND YOURGAME_LIBS
  glfw
)
