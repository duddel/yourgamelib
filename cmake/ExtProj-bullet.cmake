#
# included by PrepareExternalProjects.cmake
#
file(APPEND ${YOURGAME_EXT_PROJ_OUT_FILE}
"list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/bullet/src
)
set(BULLET_VERSION 2.89)
set(BULLET_PHYSICS_SOURCE_DIR ${YOURGAME_EXT_PROJ_CODE_DIR}/bullet)
add_subdirectory(${YOURGAME_EXT_PROJ_CODE_DIR}/bullet/src/BulletInverseDynamics)
add_subdirectory(${YOURGAME_EXT_PROJ_CODE_DIR}/bullet/src/BulletSoftBody)
add_subdirectory(${YOURGAME_EXT_PROJ_CODE_DIR}/bullet/src/BulletCollision)
add_subdirectory(${YOURGAME_EXT_PROJ_CODE_DIR}/bullet/src/BulletDynamics)
add_subdirectory(${YOURGAME_EXT_PROJ_CODE_DIR}/bullet/src/LinearMath)
add_subdirectory(${YOURGAME_EXT_PROJ_CODE_DIR}/bullet/src/Bullet3Common)
list(APPEND YOURGAME_LIBS
  BulletInverseDynamics
  BulletSoftBody
  BulletDynamics
  BulletCollision
  LinearMath
  Bullet3Common
)

# additional utils for this library, provided by yourgamelib:
list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_UTILS_SRC_DIR}/bulletenv.cpp
)
list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_UTILS_INC_DIR}/bullet
)
") # file()
