sight_add_target(module_ui_vizTest TYPE TEST)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(module_ui_vizTest PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)

target_include_directories(module_ui_vizTest PRIVATE ${CMAKE_BINARY_DIR}/modules/ui/viz/include)

add_dependencies(module_ui_vizTest data geometry_data module_service module_ui_base module_ui_viz)

target_link_libraries(module_ui_vizTest PUBLIC core utestData viz_scene3d)
