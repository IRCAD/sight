sight_add_target( service TYPE LIBRARY PCH pchData WARNINGS_AS_ERRORS ON )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      activity
                      core
                      data
)
