sight_add_target( module_ui_viz TYPE MODULE )


find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

add_dependencies(${FWPROJECT_NAME} 
                 module_ui_base
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      io_base
                      geometry_data
                      module_ui_qt
                      service
                      viz_scene3d
                      ui_base
)
