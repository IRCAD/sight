sight_add_target(module_viz_scene2d TYPE MODULE)

find_package(Qt6 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Widgets)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

target_link_libraries(
    ${SIGHT_TARGET}
    PUBLIC core
           data
           ui_qt
           viz
           viz_scene2d
           service
           ui
)

add_dependencies(${SIGHT_TARGET} module_ui_qt)
