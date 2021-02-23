fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
                 module_ui_base
                 module_ui_qt
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
                      ui_qml
)
