#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/miniaudio
)

# additional utils for this library, provided by yourgamelib:
list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_UTILS_SRC_DIR}/audioplayer.cpp
)
list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_UTILS_INC_DIR}/miniaudio
)
") # file()
