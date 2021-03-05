sight_add_target( serviceTest TYPE TEST WARNINGS_AS_ERRORS ON )


add_dependencies(serviceTest 
                 data
                 module_service
)

target_link_libraries(serviceTest PUBLIC 
                      core
                      utestData
                      data
                      service
)
