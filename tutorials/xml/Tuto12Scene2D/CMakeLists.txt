sight_add_target( Tuto12Scene2D TYPE APP )



add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_qt
                 module_service
                 module_ui_base
                 module_viz_scene2d
)
