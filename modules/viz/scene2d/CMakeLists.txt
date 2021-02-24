sight_add_target( module_viz_scene2d TYPE MODULE PRIORITY 2 )


find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)



target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      ui_qt
                      viz_base
                      viz_scene2d
                      service
                      ui_base
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
