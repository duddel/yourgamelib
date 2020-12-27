#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"set(qu3e_version 1.1)
# we only need the static library, and the PUBLIC include directory,
# which is not set for the qu3e target in the subdirectory
set(qu3e_build_shared OFF CACHE BOOL \"Build qu3e shared libraries\")
set(qu3e_build_static ON CACHE BOOL \"Build qu3e static libraries\")
set(qu3e_build_demo OFF CACHE BOOL \"Build qu3e demo\")
add_subdirectory(${YOURGAME_EXT_PROJ_CODE_DIR}/qu3e/src)
list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/qu3e/src
)
list(APPEND YOURGAME_LIBS
  qu3e
)
") # file()
