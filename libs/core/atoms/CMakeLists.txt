sight_target( atoms TYPE LIBRARY )


target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DCAMP_COMPILATION")

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
)
