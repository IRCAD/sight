sight_add_target(module_ui_viz_ut TYPE TEST)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(module_ui_viz_ut PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)

target_include_directories(module_ui_viz_ut PRIVATE ${CMAKE_BINARY_DIR}/modules/ui/viz/include)

add_dependencies(module_ui_viz_ut data geometry_data module_service module_ui module_ui_viz)

target_link_libraries(module_ui_viz_ut PUBLIC core utest_data viz_scene3d)
