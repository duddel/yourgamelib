#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/flecs
)
list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/flecs/flecs.c
)
") # file()
