sight_add_target(scene2DTest TYPE TEST WARNINGS_AS_ERRORS OFF)

add_dependencies(scene2DTest data module_service module_viz_scene2d)

target_link_libraries(scene2DTest PUBLIC utestData data service)
