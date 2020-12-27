cmake_minimum_required(VERSION 3.6)

project(ygTest)
enable_testing()

# enforce C++11 and disable extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# test: AssetManager
add_executable(${CMAKE_PROJECT_NAME}-assetmanager
  ${CMAKE_CURRENT_SOURCE_DIR}/assetmanager.cpp
)
target_include_directories(${CMAKE_PROJECT_NAME}-assetmanager PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include/toolbox
)
add_test(
  NAME ${CMAKE_PROJECT_NAME}-assetmanager
  COMMAND ${CMAKE_PROJECT_NAME}-assetmanager
)
