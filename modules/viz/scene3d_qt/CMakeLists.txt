sight_add_target(module_viz_scene3d_qt TYPE MODULE)

find_package(Qt6 QUIET COMPONENTS Widgets OpenGLWidgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE Qt6::Widgets Qt6::OpenGLWidgets)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

add_dependencies(${SIGHT_TARGET} module_viz_scene3d module_service module_ui_icons)

target_link_libraries(${SIGHT_TARGET} PUBLIC core data io_opencv ui ui_qt viz_scene3d)
