sight_add_target( atomsTest TYPE TEST )


target_compile_definitions(atomsTest PRIVATE "-DCAMP_COMPILATION")

target_link_libraries(atomsTest PUBLIC 
                      core
                      atoms
                      utestData
)
