sight_add_target( viz_base TYPE LIBRARY PCH pchService )

target_link_libraries(viz_base PUBLIC 
                      core
                      ui_base
                      service
)
