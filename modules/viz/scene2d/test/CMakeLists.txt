sight_target( scene2DTest TYPE TEST )


add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
                 module_viz_scene2d
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      utestData
                      data
                      service
)
