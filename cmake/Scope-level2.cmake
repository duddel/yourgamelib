list(APPEND YOURGAME_EXT_PROJ_LIST
  "glm"
  "miniaudio"
  "stb"
  "tinyobjloader"
)

list(APPEND YOURGAME_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/audioplayer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/camera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/glbuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/glconventions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/glgeometry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/glshader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/glshape.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/gltexture2d.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/level2.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/level2/trafo.cpp
)

list(APPEND YOURGAME_INC_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/level2
)
