sight_add_target( module_viz_scene2d TYPE MODULE PRIORITY 2 WARNINGS_AS_ERRORS OFF)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(module_viz_scene2d PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)

set_target_properties(module_viz_scene2d PROPERTIES AUTOMOC TRUE)

target_link_libraries(module_viz_scene2d PUBLIC 
                      core
                      data
                      ui_qt
                      viz_base
                      viz_scene2d
                      service
                      ui_base
)

add_dependencies(module_viz_scene2d module_ui_qt)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
