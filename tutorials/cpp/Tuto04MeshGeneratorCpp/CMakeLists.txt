fwLoadProperties()


add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_service
                 module_io_matrix
                 module_io_vtk
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      geometry_data
                      service
                      ui_base
)
