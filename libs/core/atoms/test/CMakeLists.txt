sight_add_target( atomsTest TYPE TEST )


target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DCAMP_COMPILATION")

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      atoms
                      utestData
)
