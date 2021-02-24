sight_add_target( viz_base TYPE LIBRARY PCH pchServices )

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      ui_base
                      service
)
