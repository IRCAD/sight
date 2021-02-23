fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 module_data
                 data
                 module_ui_base
                 module_ui_qt
                 module_io_matrix
                 module_io_vtk
                 module_service
                 module_filter_mesh
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_ui_icons
                 fwlauncher
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
)
