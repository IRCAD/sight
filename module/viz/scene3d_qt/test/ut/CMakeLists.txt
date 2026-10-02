sight_add_target(module_viz_scene3d_qt_ut TYPE TEST)

find_package(Qt6 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE Qt6::Widgets)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")
target_include_directories(${SIGHT_TARGET} PRIVATE ${PROJECT_SOURCE_DIR})

add_dependencies(${SIGHT_TARGET} module_viz_scene3d_qt)

target_link_libraries(${SIGHT_TARGET} PUBLIC core viz_scene3d)
