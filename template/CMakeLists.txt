# initialized from yourgamelib commit:
# {{ YOURGAME_COMMIT_SHA_INIT }}

cmake_minimum_required(VERSION 3.6)
find_package(Git)

project({{ YOURGAME_PROJECT_NAME }})

# enforce C++11 and disable extensions
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# the initial value of YOURGAME_COMMIT_SHA_REQUESTED is the commit of yourgamelib
# during initialization of this project.
# it should be adjusted if another (newer) version of yourgamelib is supported.
set(YOURGAME_COMMIT_SHA_REQUESTED {{ YOURGAME_COMMIT_SHA_INIT }})

# determine the source location of the yourgame library code:
# changing the following line to
#   if(YES)
# lets CMake clone the yourgame library to the requested commit (via GitHub).
# otherwise, the original path, where this project has been initialized from, is used.
if(NO)
  if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yourgamelib)
    message("${CMAKE_CURRENT_SOURCE_DIR}/yourgamelib exists, leaving unchanged")
  else()
    # clone yourgame library
    execute_process(COMMAND "${GIT_EXECUTABLE}" clone https://github.com/duddel/yourgamelib
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    # reset yourgame clone to the requested commit
    execute_process( COMMAND "${GIT_EXECUTABLE}" reset "${YOURGAME_COMMIT_SHA_REQUESTED}" --hard
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
{%- for MY_SRC in YOURGAME_MY_SOURCES %}
  ${CMAKE_CURRENT_SOURCE_DIR}/{{ MY_SRC }}
{%- endfor -%}
)

if(YOURGAME_PLATFORM STREQUAL "android")
  add_library(${CMAKE_PROJECT_NAME} SHARED
    ${MYGAME_SRC_FILES}
  )
  set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate"
  )
  # make sure assets/ gets copied into the android app
  # todo: convincing the Android Studio project to use ${CMAKE_CURRENT_SOURCE_DIR}/assets
  #       as the assets directory for the app is preferred, but unsolved.
  add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_SOURCE_DIR}/android/app/src/main/assets"
  )
endif()

if(YOURGAME_PLATFORM STREQUAL "web" OR YOURGAME_PLATFORM STREQUAL "desktop")
  add_executable(${CMAKE_PROJECT_NAME}
    ${MYGAME_SRC_FILES}
  )
endif()

if(YOURGAME_PLATFORM STREQUAL "desktop")
  # create savefiles/ beside the executable
  add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    "$<TARGET_FILE_DIR:${CMAKE_PROJECT_NAME}>/savefiles"
  )

  if(CPACK_GENERATOR)
    install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION .)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION .) # copy assets/
    install(DIRECTORY DESTINATION ./savefiles) # create empty savefiles/
    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CMAKE_PROJECT_NAME})
    include(CPack)
  endif()
endif()

if(YOURGAME_PLATFORM STREQUAL "web")
  # setting the output file extension to .html lets emscripten
  # generate an additional .html file along the .js and .wasm
  set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".html")
  # enforce GLFW v3 API (default is v2)
  # enforce WEB GL v2 (default is v1)
  # ALLOW_MEMORY_GROWTH=1 was added after commit bd08b66, where initializing
  # of flecs caused the web app to crash
  # todo: what exactly does ALLOW_MEMORY_GROWTH mean?
  set(C_CXX_FLAGS_FOR_EM
    "-s USE_GLFW=3 \
    -s USE_WEBGL2=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/shell.html")
  # make sure assets/ gets copied into the wasm file system,
  # but only if any asset files exist
  file(GLOB ASSET_FILES ${CMAKE_CURRENT_SOURCE_DIR}/assets/*.*)
  if(ASSET_FILES)
    set(C_CXX_FLAGS_FOR_EM
      "${C_CXX_FLAGS_FOR_EM} \
      --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets")
  endif()

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_CXX_FLAGS_FOR_EM}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${C_CXX_FLAGS_FOR_EM}")
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
  yourgame
)
