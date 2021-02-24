sight_add_target( dataTest TYPE TEST PCH pchCamp WARNINGS_AS_ERRORS ON )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      atoms
                      core
                      utestData
                      data
)
