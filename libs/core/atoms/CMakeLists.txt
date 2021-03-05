sight_add_target( atoms TYPE LIBRARY )


target_compile_definitions(atoms PRIVATE "-DCAMP_COMPILATION")

target_link_libraries(atoms PUBLIC 
                      core
                      data
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
