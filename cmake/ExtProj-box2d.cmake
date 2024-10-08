list(APPEND YOURGAME_INC_DIRS
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/include
)

list(APPEND YOURGAME_SRC_FILES
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_broad_phase.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_chain_shape.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_circle_shape.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_collide_circle.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_collide_edge.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_collide_polygon.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_collision.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_distance.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_dynamic_tree.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_edge_shape.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_polygon_shape.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/collision/b2_time_of_impact.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/common/b2_block_allocator.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/common/b2_draw.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/common/b2_math.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/common/b2_settings.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/common/b2_stack_allocator.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/common/b2_timer.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_body.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_chain_circle_contact.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_chain_polygon_contact.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_circle_contact.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_contact.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_contact_manager.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_contact_solver.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_distance_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_edge_circle_contact.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_edge_polygon_contact.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_fixture.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_friction_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_gear_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_island.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_motor_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_mouse_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_polygon_circle_contact.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_polygon_contact.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_prismatic_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_pulley_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_revolute_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_weld_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_wheel_joint.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_world.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/dynamics/b2_world_callbacks.cpp
  ${YOURGAME_EXT_PROJ_CODE_DIR}/box2d/src/rope/b2_rope.cpp
)
