sight_add_target( Tuto15Matrix TYPE APP )



add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_viz_scene3d
                 module_viz_scene3dQt
                 data
                 module_service
                 module_ui_base
                 module_io_matrix
                 module_io_vtk
                 module_io_atoms
                 module_geometry_base
)
