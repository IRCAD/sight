sight_add_target(module_ui_viz_ut TYPE TEST)

find_package(Qt6 QUIET COMPONENTS Gui REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE Qt6::Gui)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

target_include_directories(${SIGHT_TARGET} PRIVATE ${CMAKE_BINARY_DIR}/module/ui/viz/include)

add_dependencies(${SIGHT_TARGET} data geometry_data module_service module_ui module_ui_viz)

target_link_libraries(${SIGHT_TARGET} PUBLIC core utest_data viz_scene3d)
