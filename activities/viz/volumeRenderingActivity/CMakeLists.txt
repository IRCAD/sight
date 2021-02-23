sight_target( volumeRenderingActivity TYPE MODULE )




add_dependencies(${FWPROJECT_NAME} 
                 2DVisualizationActivity
                 3DVisualizationActivity
                 data
                 module_ui_base
                 module_ui_qt
                 module_data
                 module_memory
                 module_viz_scene3d
                 module_viz_scene2d
                 module_ui_icons
                 config_viz_image
                 config_ui_datamanager
                 module_service
)
