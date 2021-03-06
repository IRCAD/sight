sight_add_target( atoms TYPE LIBRARY )


target_compile_definitions(atoms PRIVATE "-DCAMP_COMPILATION")

target_link_libraries(atoms PUBLIC 
                      core
                      data
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
