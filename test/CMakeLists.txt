# initialized from yourgamelib commit:
# 06d229bc4b0113cef559a453a754c1aa1ababba4

cmake_minimum_required(VERSION 3.6)
find_package(Git)

project(ygtest VERSION 0.0.1)
enable_testing()

# tests run on desktop platform only
set(YOURGAME_PLATFORM "desktop")

# activate CLI mode for tests (no window, no GL)
list(APPEND YOURGAME_COMPILE_DEFS
  YOURGAME_CLIMODE
)

# enforce C++11 and disable extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# the initial value of YOURGAME_GIT_COMMIT_REQUESTED is the commit of yourgamelib
# during initialization of this project.
# it should be adjusted if another (newer) version of yourgamelib is supported.
set(YOURGAME_GIT_COMMIT_REQUESTED 06d229bc4b0113cef559a453a754c1aa1ababba4)

# get git commit sha and test it against the requested commit of the project:
# git describe usually returns the name of an annotated tag, but with --match="",
# no tag names match, falling back (--always) to 40 digit sha (--abbrev=40),
# appended by -dirty, if the working copy has modifications (--dirty).
execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --match=\"\" --always --abbrev=40 --dirty
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE MYGAME_GIT_COMMIT
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# get the date of the git commit
execute_process(COMMAND
  "${GIT_EXECUTABLE}" log -1 --format=%ad --date=iso8601
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  OUTPUT_VARIABLE MYGAME_GIT_DATE
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# generate source file with version information
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/mygame_version.cpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/mygame_version.cpp" @ONLY
)

# external projects
list(APPEND YOURGAME_EXT_PROJ_LIST
  "bullet"
  "doctest"
)

# add the actual yourgame library as a subdirectory
add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_CURRENT_BINARY_DIR}/libyourgame
)

# sources
list(APPEND MYGAME_SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/mygame_version.cpp # generated above
)

# inc dirs (internal)
list(APPEND MYGAME_INC_DIRS_PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# glob all .cpp files in testcases/ and add to build
file(GLOB YGTEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/testcases/*.cpp)

add_executable(${CMAKE_PROJECT_NAME}
  ${MYGAME_SRC_FILES}
  ${YGTEST_FILES}
)

target_include_directories(${CMAKE_PROJECT_NAME}
  PRIVATE ${MYGAME_INC_DIRS_PRIVATE}
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
  yourgame
)

# register test executable to be run via ctest
add_test(
  NAME ${CMAKE_PROJECT_NAME}
  COMMAND ${CMAKE_PROJECT_NAME}
)
