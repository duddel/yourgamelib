cmake_minimum_required(VERSION 3.6)

project(framework)

# enforce C++11 and disable extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../..
  ${CMAKE_CURRENT_BINARY_DIR}/libyourgame
)

if(YOURGAME_PLATFORM STREQUAL "android")
  add_library(${CMAKE_PROJECT_NAME} SHARED
    mygame.cpp
  )
  set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate"
  )
  # make sure assets/ gets copied into the android app
  add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_SOURCE_DIR}/android/app/src/main/assets"
  )
endif()

if(YOURGAME_PLATFORM STREQUAL "wasm" OR YOURGAME_PLATFORM STREQUAL "desktop")
  add_executable(${CMAKE_PROJECT_NAME}
    mygame.cpp
  )
endif()

if(YOURGAME_PLATFORM STREQUAL "desktop")
  # make sure assets/ gets copied beside the executable
  add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/assets"
  )
endif()

if(YOURGAME_PLATFORM STREQUAL "wasm")
  # setting the output file extension to .html lets emscripten
  # generate an additional .html file along the .js and .wasm
  set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".html")
  # enforce GLFW v3 API (default is v2)
  # make sure assets/ gets copied into the wasm file system
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s USE_WEBGL2=1 --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
  yourgame
)
