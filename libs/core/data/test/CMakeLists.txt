sight_add_target( dataTest TYPE TEST PCH pchCamp WARNINGS_AS_ERRORS ON )


target_link_libraries(dataTest PUBLIC 
                      atoms
                      core
                      utestData
                      data
)
