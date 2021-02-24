sight_add_target( module_activityTest TYPE TEST )





target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
                      activity
)
