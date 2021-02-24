sight_add_target( Tuto10LaunchBasicConfig TYPE APP )



add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_io_matrix
                 module_io_vtk
                 module_viz_sample
)
