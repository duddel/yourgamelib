cmake_minimum_required(VERSION 3.6)
find_package(Git)

# yourgame library project + version
project(yourgame VERSION 0.0.0)

# enforce C++11 and disable extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# todo: allow custom gl versions
set(YOURGAME_GL_MAJOR 3)
set(YOURGAME_GL_MINOR 3)
set(YOURGAME_GLES_MAJOR 3)
set(YOURGAME_GLES_MINOR 0)

# get git commit sha and test it against the requested commit of the project:
# git describe usually returns the name of an annotated tag, but with --match="",
# no tag names match, falling back (--always) to 40 digit sha (--abbrev=40),
# appended by -dirty, if the working copy has modifications (--dirty).
execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --match=\"\" --always --abbrev=40 --dirty
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE YOURGAME_GIT_COMMIT_ACTUAL
  OUTPUT_STRIP_TRAILING_WHITESPACE)

if(YOURGAME_GIT_COMMIT_REQUESTED STREQUAL YOURGAME_GIT_COMMIT_ACTUAL)
  message("The requested yourgamelib commit ${YOURGAME_GIT_COMMIT_REQUESTED} \
  matches yourgamelib in ${CMAKE_CURRENT_SOURCE_DIR}")
else()
  message(WARNING "The requested yourgamelib commit ${YOURGAME_GIT_COMMIT_REQUESTED} \
  does NOT MATCH yourgamelib in ${CMAKE_CURRENT_SOURCE_DIR} \
  YOURGAME_GIT_COMMIT_ACTUAL: ${YOURGAME_GIT_COMMIT_ACTUAL}")  
endif()

# get the date of the git commit
execute_process(COMMAND
  "${GIT_EXECUTABLE}" log -1 --format=%ad --date=iso8601
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE YOURGAME_GIT_DATE
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# generate source file with version information
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/version.cpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/version.cpp" @ONLY
)

# toolbox
if(NOT DEFINED YOURGAME_BARE)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Toolbox.cmake)
endif()

# sources
list(APPEND YOURGAME_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/audio.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/file.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/input.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/log.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/time.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/version.cpp # generated above
)
if(YOURGAME_PLATFORM STREQUAL "desktop" OR YOURGAME_PLATFORM STREQUAL "web")
  list(APPEND YOURGAME_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_desktop_web/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_desktop_web/input.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_desktop_web/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_desktop_web/yourgame.cpp
  )
elseif(YOURGAME_PLATFORM STREQUAL "android")
  list(APPEND YOURGAME_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_android/file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_android/input.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_android/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform_android/yourgame.cpp
  )
endif()

# inc dirs (api)
list(APPEND YOURGAME_INC_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/base
)
# inc dirs (internal)
list(APPEND YOURGAME_INC_DIRS_PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/base_internal
)

# preprocessor definition to describe the platform
string(TOUPPER ${YOURGAME_PLATFORM} platform_upper)
list(APPEND YOURGAME_COMPILE_DEFS
  YOURGAME_PLATFORM_${platform_upper}
)

# external projects and yourgame config
list(APPEND YOURGAME_EXT_PROJ_LIST
  "easyloggingpp"
  "dirent"
  "miniz"
)
if(YOURGAME_PLATFORM STREQUAL "desktop")
  list(APPEND YOURGAME_EXT_PROJ_LIST
    "glfw"
    "whereami"
  )
  if(YOURGAME_FORCE_GLES)
    set(YOURGAME_PLATFORM_GL_API "gles")
  else()
    set(YOURGAME_PLATFORM_GL_API "gl")
  endif()
elseif(YOURGAME_PLATFORM STREQUAL "web")
  set(YOURGAME_PLATFORM_GL_API "gles")
elseif(YOURGAME_PLATFORM STREQUAL "android")
  set(YOURGAME_PLATFORM_GL_API "gles")
  list(APPEND YOURGAME_EXT_PROJ_LIST
    "android-native-app"
  )
else()
  message(FATAL_ERROR "YOURGAME_PLATFORM is not defined or unknown: ${YOURGAME_PLATFORM}")
endif()

# gl api, version, lib
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
  if(YOURGAME_GLES_MAJOR EQUAL 1)
    list(APPEND YOURGAME_LIBS GLESv1_C)
  elseif(YOURGAME_GLES_MAJOR EQUAL 2)
    list(APPEND YOURGAME_LIBS GLESv2)
  elseif(YOURGAME_GLES_MAJOR EQUAL 3)
    if(YOURGAME_PLATFORM STREQUAL "web")
      list(APPEND YOURGAME_LIBS GLESv2)
    else()
      list(APPEND YOURGAME_LIBS GLESv3)
    endif()
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

# external projects:
# the modules (ExtProj-<proj>.cmake) may write CMake commands,
# such as add_subdirectory(), and may append items to these lists:
# YOURGAME_INC_DIRS, YOURGAME_SRC_FILES, YOURGAME_LIBS, YOURGAME_COMPILE_DEFS
set(YOURGAME_EXT_PROJ_CODE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
foreach(proj ${YOURGAME_EXT_PROJ_LIST})
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ExtProj-${proj}.cmake)
  # add a compile definition for each available external project,
  # replace improper characters from ${extproj} by an underscore (_)
  string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" proj ${proj})
  list(APPEND YOURGAME_COMPILE_DEFS YOURGAME_EXTPROJ_${proj})
endforeach()

# add the yourgame library target
add_library(${PROJECT_NAME}
  ${YOURGAME_SRC_FILES}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE ${YOURGAME_INC_DIRS_PRIVATE}
  PUBLIC ${YOURGAME_INC_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
  ${YOURGAME_LIBS}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
  ${YOURGAME_COMPILE_DEFS}
)
