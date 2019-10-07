cmake_minimum_required(VERSION 3.7)

# enforce C++11 and disable extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# todo: allow custom gl versions
set(YOURGAME_GL_MAJOR 3)
set(YOURGAME_GL_MINOR 3)
set(YOURGAME_GLES_MAJOR 2)
set(YOURGAME_GLES_MINOR 0)

# common yourgame source files
list(APPEND YOURGAME_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/timer.cpp
)

# common yourgame external projects
list(APPEND YOURGAME_EXT_PROJ_LIST
  "easyloggingpp"
)

# common yourgame inc dirs
list(APPEND YOURGAME_INC_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# yourgame platforms
if(YOURGAME_PLATFORM STREQUAL "desktop")
  set(YOURGAME_PLATFORM_INTEGRATION "glfw")
  if(YOURGAME_FORCE_GLES)
    set(YOURGAME_PLATFORM_GL_API "gles")
  else()
    set(YOURGAME_PLATFORM_GL_API "gl")
  endif()
  list(APPEND YOURGAME_EXT_PROJ_LIST
    "whereami"
  )
  list(APPEND YOURGAME_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/yourgame_desktop.cpp
  )
else()
  message(FATAL_ERROR "YOURGAME_PLATFORM is not defined or unknown: ${YOURGAME_PLATFORM}")
endif()

# platform integration
if(YOURGAME_PLATFORM_INTEGRATION STREQUAL "glfw")
  list(APPEND YOURGAME_EXT_PROJ_LIST "glfw")
endif()

# gl api, version, library
if(YOURGAME_PLATFORM_GL_API STREQUAL "gl")
  list(APPEND YOURGAME_COMPILE_DEFS
    YOURGAME_GL_API_GL
    YOURGAME_GL_MAJOR=${YOURGAME_GL_MAJOR}
    YOURGAME_GL_MINOR=${YOURGAME_GL_MINOR}
  )
  set(YOURGAME_PLATFORM_GL_EXT_LOADER "glad")
  find_package(OpenGL REQUIRED)
  list(APPEND YOURGAME_INC_DIRS
    ${OPENGL_INCLUDE_DIR}
  )
  list(APPEND YOURGAME_LIBS
    ${OPENGL_LIBRARIES}
  )
elseif(YOURGAME_PLATFORM_GL_API STREQUAL "gles")
  list(APPEND YOURGAME_COMPILE_DEFS
    YOURGAME_GL_API_GLES
    YOURGAME_GL_MAJOR=${YOURGAME_GLES_MAJOR}
    YOURGAME_GL_MINOR=${YOURGAME_GLES_MINOR}
  )
  # "recommended" gles library names (might differ)
  if(YOURGAME_GLES_MAJOR EQUAL 1)
    list(APPEND YOURGAME_LIBS GLESv1_C)
  else()
    list(APPEND YOURGAME_LIBS GLESv2) # usually valid for gles v2 and v3
  endif()
endif()

# gl include
if(YOURGAME_PLATFORM_GL_API STREQUAL "gles")
  if(YOURGAME_GLES_MAJOR EQUAL 1 AND YOURGAME_GLES_MINOR EQUAL 0)
    list(APPEND YOURGAME_COMPILE_DEFS YOURGAME_GL_INCLUDE_GLES1)
  elseif(YOURGAME_GLES_MAJOR EQUAL 1 AND YOURGAME_GLES_MINOR EQUAL 1)
    list(APPEND YOURGAME_COMPILE_DEFS YOURGAME_GL_INCLUDE_GLES1)
  elseif(YOURGAME_GLES_MAJOR EQUAL 2 AND YOURGAME_GLES_MINOR EQUAL 0)
    list(APPEND YOURGAME_COMPILE_DEFS YOURGAME_GL_INCLUDE_GLES2)
  elseif(YOURGAME_GLES_MAJOR EQUAL 3 AND YOURGAME_GLES_MINOR EQUAL 0)
    list(APPEND YOURGAME_COMPILE_DEFS YOURGAME_GL_INCLUDE_GLES3)
  elseif(YOURGAME_GLES_MAJOR EQUAL 3 AND YOURGAME_GLES_MINOR EQUAL 1)
    list(APPEND YOURGAME_COMPILE_DEFS YOURGAME_GL_INCLUDE_GLES31)
  elseif(YOURGAME_GLES_MAJOR EQUAL 3 AND YOURGAME_GLES_MINOR EQUAL 2)
    list(APPEND YOURGAME_COMPILE_DEFS YOURGAME_GL_INCLUDE_GLES32)
  else()
    message(FATAL_ERROR "gles version ${YOURGAME_GLES_MAJOR}.${YOURGAME_GLES_MINOR} unknown")
  endif()
elseif(YOURGAME_PLATFORM_GL_EXT_LOADER STREQUAL "glad")
  list(APPEND YOURGAME_COMPILE_DEFS YOURGAME_GL_INCLUDE_GLAD)
else()
  message(FATAL_ERROR "no gl include set")
endif()

# gl extension loader
if(YOURGAME_PLATFORM_GL_EXT_LOADER STREQUAL "glad")
  list(APPEND YOURGAME_EXT_PROJ_LIST "glad-gl3.3-core")
  list(APPEND YOURGAME_COMPILE_DEFS
    YOURGAME_GL_EXT_LOADER_GLAD
  )
endif()

# platform specific configuration for external projects
# imgui
if("imgui" IN_LIST YOURGAME_EXT_PROJ_LIST)
  if(YOURGAME_PLATFORM_GL_EXT_LOADER STREQUAL "glad")
    list(APPEND YOURGAME_COMPILE_DEFS
      IMGUI_IMPL_OPENGL_LOADER_GLAD
    )
  endif()
endif()

# prepare external projects
# step 1: copy the script
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/PrepareExternalProjects.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/thirdparty/CMakeLists.txt" COPYONLY
)
# step 2:
# run a separate CMake instance preparing external projects.
# the original generator of the current CMake instance is intentionally
# not passed to the new CMake instance.
# to do it anyway, add this to the command below: -G "${CMAKE_GENERATOR}"
execute_process(
  COMMAND "${CMAKE_COMMAND}" .
  "-DYOURGAME_EXT_PROJ_LIST=${YOURGAME_EXT_PROJ_LIST}"
  "-DYOURGAME_EXT_PROJ_MODULE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/thirdparty"
  "-DYOURGAME_EXT_PROJ_OUT_FILE=${CMAKE_CURRENT_BINARY_DIR}/thirdparty/yourgame_ext_proj.cmake"
  RESULT_VARIABLE extProjResult
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/thirdparty"
)
if(extProjResult)
  message(FATAL_ERROR "Configure step of external projects FAILED: ${extProjResult}")
endif()
# step 3:
# run the "build step" in a separate CMake instance
execute_process(
  COMMAND "${CMAKE_COMMAND}" --build .
  RESULT_VARIABLE extProjResult
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/thirdparty"
)
if(extProjResult)
  message(FATAL_ERROR "Build step (prepare) of external projects FAILED: ${extProjResult}")
endif()
# step 4:
# include the output file
include(${CMAKE_CURRENT_BINARY_DIR}/thirdparty/yourgame_ext_proj.cmake)

# add the yourgame library target
add_library(yourgame
  ${YOURGAME_SRC_FILES}
)

target_include_directories(yourgame PUBLIC
  ${YOURGAME_INC_DIRS}
)

target_link_libraries(yourgame PUBLIC
  ${YOURGAME_LIBS}
)

target_compile_definitions(yourgame PUBLIC
  ${YOURGAME_COMPILE_DEFS}
)
