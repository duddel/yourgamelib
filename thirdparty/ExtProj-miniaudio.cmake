#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_MODULE_DIR}/miniaudio
)
list(APPEND YOURGAME_COMPILE_DEFS
  MA_NO_DECODING
  MA_NO_STDIO
)
") # file()
