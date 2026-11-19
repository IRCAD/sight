# A real render window: as a GUI test it has its own display, and leaves the foreground of the user alone.
sight_add_target(viz_scene3d_ut TYPE GUI_TEST PROFILE NONE)

find_package(Qt6 QUIET COMPONENTS Core REQUIRED)
target_link_libraries(viz_scene3d_ut PUBLIC core utest_data data viz_scene3d PRIVATE Qt6::Core)

add_dependencies(viz_scene3d_ut module_viz_scene3d_test)
