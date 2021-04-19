#
# included by PrepareExternalProjects.cmake
#
# dirent.h provides the dirent interface for Windows. on Linux/UNIX, native dirent.h
# is used by omitting the include directory (/dirent/include) here.
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"if(WIN32)
  list(APPEND YOURGAME_INC_DIRS
    ${YOURGAME_EXT_PROJ_CODE_DIR}/dirent/include
  )
endif()
") # file()
