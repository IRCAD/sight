fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      service
)
