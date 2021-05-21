sight_add_target(viz_qt3dTest TYPE TEST WARNINGS_AS_ERRORS OFF)

target_link_libraries(viz_qt3dTest PUBLIC 
                      data
                      viz_qt3d
                      utestData
)
