sight_add_target( ExNotifications TYPE APP )


add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_service
                 module_ui_base
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      ui_base
)
