sight_add_target( module_sync TYPE MODULE )


target_link_libraries(module_sync PUBLIC 
                      core
                      data
                      service
                      geometry_data
)
