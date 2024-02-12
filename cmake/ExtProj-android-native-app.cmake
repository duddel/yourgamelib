list(APPEND YOURGAME_INC_DIRS
  ${ANDROID_NDK}/sources/android/native_app_glue
)

list(APPEND YOURGAME_SRC_FILES
  ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

list(APPEND YOURGAME_LIBS
  android
  EGL
  log
  OpenSLES
)
