sight_target( ExActivities TYPE APP )


add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_activity
                 data
                 module_service
                 module_ui_base
                 module_ui_flaticons
                 module_io_atoms
                 module_io_vtk
                 module_io_activity
                 2DVisualizationActivity
                 volumeRenderingActivity
                 config_ui_activity
                 module_viz_sample
)
