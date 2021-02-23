fwLoadProperties()
add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_ui_base
                 module_ui_qt
                 module_viz_qt3d
                 data
                 module_service
                 module_ui_base
                 module_io_vtk
                 module_viz_qt3d
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
)
