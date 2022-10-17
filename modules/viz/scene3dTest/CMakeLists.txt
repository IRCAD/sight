sight_add_target(module_viz_scene3d_test TYPE MODULE)

add_definitions("-DQT_NO_KEYWORDS")

find_package(Qt5 QUIET COMPONENTS Gui REQUIRED)

target_link_libraries(module_viz_scene3d_test PUBLIC Qt5::Gui viz_scene3d)
