sight_add_target( Tuto05Mesher TYPE APP )

add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_qt
                 data
                 module_service
                 module_ui_base
                 module_io_vtk
                 module_viz_sample
                 module_filter_mesh
)
