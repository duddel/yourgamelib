#
# included by PrepareExternalProjects.cmake
#
# dirent.h provides the dirent interface for Windows. on Linux/UNIX, native dirent.h
# is used by omitting the include directory (/dirent/include) here.
# todo: even if mingw is used, the following if() reports MSVC,
#       but dirent.h might not be required if using mingw.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/dirent/include
)
") # file()
endif()
