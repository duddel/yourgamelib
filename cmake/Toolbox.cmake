list(APPEND YOURGAME_EXT_PROJ_LIST
  "glm"
  "imgui"
  "json"
  "stb"
  "tinyobjloader"
)

list(APPEND YOURGAME_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/conventions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/drawing.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/framebuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/geometry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/lightsource.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/loading.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/particles.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/shader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/shape.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/spritegrid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gl/texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/math/camera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/math/particles.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/math/trafo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/util/motion.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/util/physenv.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/headerlibs.cpp
)

list(APPEND YOURGAME_INC_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/toolbox
)

list(APPEND YOURGAME_COMPILE_DEFS
  YOURGAME_TOOLBOX
)
