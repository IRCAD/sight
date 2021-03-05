sight_add_target( viz_base TYPE LIBRARY PCH pchServices )

target_link_libraries(viz_base PUBLIC 
                      core
                      ui_base
                      service
)
