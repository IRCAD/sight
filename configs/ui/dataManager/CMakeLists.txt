sight_add_target( config_ui_datamanager TYPE MODULE)


add_dependencies(config_ui_datamanager
                 module_ui_base
                 module_ui_qt
                 module_data
                 module_ui_icons
                 module_service
                 data
)
