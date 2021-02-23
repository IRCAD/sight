fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_ui_qt
                 module_service
                 module_ui_base
                 module_io_vtk
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
)
