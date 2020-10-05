#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/choreograph
  ${YOURGAME_EXT_PROJ_CODE_DIR}/choreograph/choreograph
)
list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/choreograph/choreograph/Cue.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/choreograph/choreograph/Timeline.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/choreograph/choreograph/TimelineItem.cpp
)
") # file()
