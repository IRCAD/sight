sight_add_target( activity_viz_negato TYPE MODULE)

add_dependencies(activity_viz_negato
                 data
                 module_activity
                 module_ui_base
                 module_ui_qt
                 module_data
                 module_memory
                 module_service
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_ui_icons
)
