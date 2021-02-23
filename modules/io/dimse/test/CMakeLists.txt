fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
                 module_ui_base
                 module_io_dimse
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      service
                      io_dimse
                      utestData
)
