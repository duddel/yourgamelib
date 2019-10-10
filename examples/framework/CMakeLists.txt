cmake_minimum_required(VERSION 3.7)

# enforce C++11 and disable extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../..
  ${CMAKE_CURRENT_BINARY_DIR}/libyourgame
)

add_executable(framework
  main.cpp
  mygame.cpp
)

if(YOURGAME_PLATFORM STREQUAL "wasm")
  set_target_properties(framework PROPERTIES SUFFIX ".html")
endif()

target_link_libraries(framework PRIVATE
  yourgame
)
