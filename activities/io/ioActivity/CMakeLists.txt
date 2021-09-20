sight_add_target( module_io_activity TYPE MODULE)

add_dependencies(module_io_activity
                 module_activity
                 module_data
                 module_ui_base
                 module_ui_qt
                 module_ui_icons
                 data
                 module_io_atoms
)
