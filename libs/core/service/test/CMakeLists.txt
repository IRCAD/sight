sight_add_target( serviceTest TYPE TEST WARNINGS_AS_ERRORS ON )


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
