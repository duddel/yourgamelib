cmake_minimum_required(VERSION 3.10)

# enforce C++11 and disable extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# restrict this example to desktop platform only
set(YOURGAME_PLATFORM desktop)

add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../..
  ${CMAKE_CURRENT_BINARY_DIR}/libyourgame
)

add_executable(mini-gl
  main.cpp
)

target_link_libraries(mini-gl PRIVATE
  yourgame
)
