list(APPEND YOURGAME_EXT_PROJ_LIST
  "glm"
  "imgui"
  "json"
  "stb"
  "tinyobjloader"
)

list(APPEND YOURGAME_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/camera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/glbuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/glconventions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gldrawing.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/glframebuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/glgeometry.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gllightsource.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/glloading.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/glparticles.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/glshader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/glshape.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/glspritegrid.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gltexture2d.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/gltextureatlas.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/headerlibs.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/particles.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/toolbox/trafo.cpp
)

list(APPEND YOURGAME_INC_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/toolbox
)
