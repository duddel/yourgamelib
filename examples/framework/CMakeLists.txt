cmake_minimum_required(VERSION 3.10)

# restrict this example to desktop platform only
set(YOURGAME_PLATFORM desktop)

add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../..
  ${CMAKE_CURRENT_BINARY_DIR}/libyourgame
)

add_executable(framework
  main.cpp
)

target_link_libraries(framework PRIVATE
  yourgame
)
