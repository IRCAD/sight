sight_target( module_service TYPE MODULE )


add_dependencies(${FWPROJECT_NAME} 
                 data
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      service
)
