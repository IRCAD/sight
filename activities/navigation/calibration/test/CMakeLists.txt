sight_add_target( activity_navigation_calibrationTest TYPE TEST )





add_dependencies(${FWPROJECT_NAME} 
                 module_activity
                 activity_navigation_calibration
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      activity
                      core
                      utestData
                      data
)
