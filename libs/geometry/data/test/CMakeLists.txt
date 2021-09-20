sight_add_target( geometry_dataTest TYPE TEST WARNINGS_AS_ERRORS OFF)

target_link_libraries(geometry_dataTest PUBLIC
                      core
                      geometry_data
                      utestData
)
