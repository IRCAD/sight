sight_add_target(module_ui_viz TYPE MODULE)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(module_ui_viz PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)

set_target_properties(module_ui_viz PROPERTIES AUTOMOC TRUE)

add_dependencies(module_ui_viz module_ui module_ui_qt)

target_link_libraries(
    module_ui_viz
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
