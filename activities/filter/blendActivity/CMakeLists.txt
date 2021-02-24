sight_add_target( blendActivity TYPE MODULE )



add_dependencies(${FWPROJECT_NAME} 
                 module_ui_base
                 module_ui_qt
                 module_memory
                 module_data
                 module_viz_scene3d
                 module_viz_scene2d
                 data
                 module_ui_icons
                 config_viz_scene2d
                 module_service
)
