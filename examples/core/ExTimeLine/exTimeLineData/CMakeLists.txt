sight_add_target( exTimeLineData TYPE LIBRARY )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
)

sight_add_shortcut(${FWPROJECT_NAME})
