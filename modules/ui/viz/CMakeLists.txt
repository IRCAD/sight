sight_add_target(module_ui_viz TYPE MODULE)

find_package(Qt6 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Widgets)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

add_dependencies(${SIGHT_TARGET} module_ui module_ui_qt)

target_link_libraries(
    ${SIGHT_TARGET}
    PUBLIC core
           data
           io
           geometry_data
           ui_qt
           service
           viz_scene3d
           ui
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
