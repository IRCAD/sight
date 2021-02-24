sight_add_target( module_io_activity TYPE MODULE )




add_dependencies(${FWPROJECT_NAME} 
                 module_activity
                 module_data
                 module_ui_base
                 module_ui_qt
                 module_ui_icons
                 data
                 module_io_atoms
)
