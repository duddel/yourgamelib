cmake_minimum_required(VERSION 3.7)

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
    main_android.cpp
    mygame.cpp
  )
  set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate"
  )
else()
  add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
    mygame.cpp
  )
endif()

if(YOURGAME_PLATFORM STREQUAL "wasm")
  # setting the output file extension to .html lets emscripten
  # generate an additional .html file along the .js and .wasm
  set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".html")
  # enforce GLFW v3 API (default is v2)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3")
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
  yourgame
)
