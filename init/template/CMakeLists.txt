# initialized from yourgamelib commit:
# {{ YOURGAME_GIT_COMMIT_INIT }}

cmake_minimum_required(VERSION 3.6)
find_package(Git)

project({{ YOURGAME_PROJECT_NAME }} VERSION 0.0.1)

# Name of the CMake target (default: name of project defined above)
set(YOURGAME_PROJECT_TARGET_NAME ${CMAKE_PROJECT_NAME})

# Name of the target output files (default: name of project defined above)
set(YOURGAME_PROJECT_OUTPUT_NAME ${CMAKE_PROJECT_NAME})

# Web platform: pack assets
# enable this option to copy assets (if any) into the wasm file system.
# only recommended if the assets are final and not to be updated.
# otherwise, assets are loaded from assets/ directory during runtime.
set(WEB_PACK_ASSETS OFF)

# Web platform: set GLFW implementation to use.
# if ON: use a more complete GLFW implementation (--use-port=contrib.glfw)
# if OFF: use the stock GLFW3 port (-sUSE_GLFW=3)
set(YOURGAME_WEB_USE_CONTRIB_GLFW ON)

# enable this option to clone yourgamelib and checkout the commit, defined
# as YOURGAME_GIT_COMMIT_REQUESTED. otherwise, the original path, where
# this project has been initialized from, is used.
set(YOURGAME_GIT_CLONE {{ YOURGAME_CLONE }})

# if YOURGAME_GIT_CLONE == ON, this is the git remote url, yourgamelib is cloned from.
set(YOURGAME_GIT_REMOTE https://github.com/duddel/yourgamelib)

# the initial value of YOURGAME_GIT_COMMIT_REQUESTED is the commit of yourgamelib
# during initialization of this project.
# it should be adjusted if another (newer) version of yourgamelib is supported.
set(YOURGAME_GIT_COMMIT_REQUESTED {{ YOURGAME_GIT_COMMIT_INIT }})

# enforce C++11 and disable extensions
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

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
  "${CMAKE_CURRENT_SOURCE_DIR}/version/mygame_version.cpp.in"
  "${CMAKE_CURRENT_BINARY_DIR}/mygame_version.cpp" @ONLY
)

# external projects
list(APPEND YOURGAME_EXT_PROJ_LIST
{%- for EXT_PROJ in YOURGAME_EXT_PROJ %}
  "{{ EXT_PROJ }}"
{%- endfor -%}
)

{%- if YOURGAME_BARE == true %}
# bare
set(YOURGAME_BARE TRUE)
{%- endif %}

# determine the source location of the yourgame library code
if(YOURGAME_GIT_CLONE)
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yourgamelib)
    message("${CMAKE_CURRENT_SOURCE_DIR}/yourgamelib exists, leaving unchanged")
  else()
    # clone yourgame library
    execute_process(COMMAND "${GIT_EXECUTABLE}" clone "${YOURGAME_GIT_REMOTE}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    # checkout the requested yourgamelib commit
    execute_process(COMMAND "${GIT_EXECUTABLE}" checkout "${YOURGAME_GIT_COMMIT_REQUESTED}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/yourgamelib"
    )
  endif()
  # use the newly cloned yourgame library path
  set(YOURGAME_LIBRARY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/yourgamelib)
else()
  # use the original yourgame library path from initialization of this project
  set(YOURGAME_LIBRARY_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/{{ YOURGAME_ROOT }})
endif()

# add the actual yourgame library as a subdirectory
add_subdirectory(
  ${YOURGAME_LIBRARY_ROOT}
  ${CMAKE_CURRENT_BINARY_DIR}/libyourgame
)

# sources
list(APPEND MYGAME_SRC_FILES
  ${CMAKE_CURRENT_BINARY_DIR}/mygame_version.cpp # generated above
)
list(APPEND MYGAME_SRC_FILES
{%- for MY_SRC in YOURGAME_MY_SOURCES %}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/{{ MY_SRC }}
{%- endfor -%}
)

# inc dirs (internal)
list(APPEND MYGAME_INC_DIRS_PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/version
)

if(YOURGAME_PLATFORM STREQUAL "android")
  add_library(${YOURGAME_PROJECT_TARGET_NAME} SHARED
    ${MYGAME_SRC_FILES}
  )
  set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate"
  )
  # make sure assets/ gets copied into the android app
  # todo: convincing the Android Studio project to use ${CMAKE_CURRENT_SOURCE_DIR}/assets
  #       as the assets directory for the app is preferred, but unsolved.
  add_custom_command(
    TARGET ${YOURGAME_PROJECT_TARGET_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_SOURCE_DIR}/build/android/app/src/main/assets"
  )
endif()

if(YOURGAME_PLATFORM STREQUAL "web" OR YOURGAME_PLATFORM STREQUAL "desktop")
  add_executable(${YOURGAME_PROJECT_TARGET_NAME}
    ${MYGAME_SRC_FILES}
  )
endif()

set_target_properties(${YOURGAME_PROJECT_TARGET_NAME} PROPERTIES OUTPUT_NAME ${YOURGAME_PROJECT_OUTPUT_NAME})

if(YOURGAME_PLATFORM STREQUAL "desktop")
  if(CPACK_GENERATOR)
    install(TARGETS ${YOURGAME_PROJECT_TARGET_NAME} DESTINATION .)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION .) # copy assets/
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${YOURGAME_PROJECT_OUTPUT_NAME})
    include(CPack)
  endif()
endif()

if(YOURGAME_PLATFORM STREQUAL "web")
  # setting the output file extension to .html lets emscripten
  # generate an additional .html file along the .js and .wasm
  set_target_properties(${YOURGAME_PROJECT_TARGET_NAME} PROPERTIES SUFFIX ".html")

  set(LINKER_FLAGS_FOR_EM
    "-sMIN_WEBGL_VERSION=2 \
-sMAX_WEBGL_VERSION=2 \
-sALLOW_MEMORY_GROWTH=1 \
-sASYNCIFY \
--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/build/shell.html")

  # Enable requested GLFW implementation
  if(YOURGAME_WEB_USE_CONTRIB_GLFW)
    set(LINKER_FLAGS_FOR_EM "${LINKER_FLAGS_FOR_EM} --use-port=contrib.glfw3")
    # This is also a compiler flag and needs to be set for the yourgame library
    target_compile_options(yourgame PUBLIC "--use-port=contrib.glfw3")
  else()
    set(LINKER_FLAGS_FOR_EM "${LINKER_FLAGS_FOR_EM} -sUSE_GLFW=3")
  endif()

  # check if there are any assets
  file(GLOB ASSET_FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/*.*)

  if(WEB_PACK_ASSETS AND ASSET_FILES)
    set(LINKER_FLAGS_FOR_EM "${LINKER_FLAGS_FOR_EM} \
--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/home/web_user/")
  endif()

  # Set final linker flags
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS_FOR_EM}")

  if(CPACK_GENERATOR)
    # these 4 files are typically created by emscripten: .html, .js, .wasm, .data.
    # the .data file is only available if any asset files have been packed (see above)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${YOURGAME_PROJECT_OUTPUT_NAME}.html DESTINATION . RENAME index.html)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${YOURGAME_PROJECT_OUTPUT_NAME}.js DESTINATION .)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${YOURGAME_PROJECT_OUTPUT_NAME}.wasm DESTINATION .)

    # pack asset files, if any.
    # either pack the .data file, if WEB_PACK_ASSETS == ON,
    # or pack separate assets/ directory, otherwise.
    if(ASSET_FILES)
      if(WEB_PACK_ASSETS)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${YOURGAME_PROJECT_OUTPUT_NAME}.data DESTINATION .)
      else()
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/ DESTINATION ./assets/)
      endif()
    endif()

    set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
    include(CPack)
  endif()
endif()

target_include_directories(${YOURGAME_PROJECT_TARGET_NAME}
  PRIVATE ${MYGAME_INC_DIRS_PRIVATE}
)

target_link_libraries(${YOURGAME_PROJECT_TARGET_NAME} PRIVATE
  yourgame
)
