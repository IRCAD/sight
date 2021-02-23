fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 module_io_vtk
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
                      service
                      ui_base
)
