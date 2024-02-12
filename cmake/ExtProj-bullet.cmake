list(APPEND YOURGAME_INC_DIRS
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
