#
# included by PrepareExternalProjects.cmake
#
# ANDROID_NDK path is set by the android sdk
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"
list(APPEND YOURGAME_INC_DIRS
  \${ANDROID_NDK}/sources/android/native_app_glue
)
list(APPEND YOURGAME_SRC_FILES
  \${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)
list(APPEND YOURGAME_LIBS
  android
  EGL
  log
)
") # file()
