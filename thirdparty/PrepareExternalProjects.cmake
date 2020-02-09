# run this script in a separate CMake instance.
# output: writes file (YOURGAME_EXT_PROJ_OUT_FILE) to be
# included by the main CMake instance
#
# YOURGAME_EXT_PROJ_LIST        - list of external projects
# YOURGAME_EXT_PROJ_MODULE_DIR  - absolute path to directory of ExtProj-<proj>.cmake modules
# YOURGAME_EXT_PROJ_OUT_FILE    - absolute path to output file
#
# the modules (ExtProj-<proj>.cmake) may write CMake commands,
# such as add_subdirectory(), and may append items to these lists:
#
# YOURGAME_INC_DIRS       - include directories
# YOURGAME_SRC_FILES      - source files
# YOURGAME_LIBS           - libraries to link
# YOURGAME_COMPILE_DEFS   - compile definitions
cmake_minimum_required(VERSION 3.6)

# write initial line to file, resets the file if it exists already
file(WRITE ${YOURGAME_EXT_PROJ_OUT_FILE} "# this file is auto-generated, do not modify!
") # file()

foreach(extproj ${YOURGAME_EXT_PROJ_LIST})
  file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE} "
#
# ${extproj}
#
") # file()
  include(${YOURGAME_EXT_PROJ_MODULE_DIR}/ExtProj-${extproj}.cmake)
  file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE} "
list(APPEND YOURGAME_COMPILE_DEFS
  YOURGAME_EXTPROJ_${extproj}
)
") # file()
endforeach()
