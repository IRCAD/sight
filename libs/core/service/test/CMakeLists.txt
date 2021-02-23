fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
                      service
)
