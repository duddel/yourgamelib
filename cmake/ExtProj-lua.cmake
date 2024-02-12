list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua
  ${YOURGAME_EXT_PROJ_CODE_DIR}/luabridge
)

# all Lua sources, except lua.c and onelua.c
list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lapi.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lauxlib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lbaselib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lcode.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lcorolib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lctype.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/ldblib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/ldebug.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/ldo.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/ldump.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lfunc.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lgc.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/linit.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/liolib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/llex.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lmathlib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lmem.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/loadlib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lobject.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lopcodes.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/loslib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lparser.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lstate.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lstring.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lstrlib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/ltable.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/ltablib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/ltests.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/ltm.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lundump.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lutf8lib.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lvm.c
  ${YOURGAME_EXT_PROJ_CODE_DIR}/lua/lzio.c
)
