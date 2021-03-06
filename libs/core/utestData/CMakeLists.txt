sight_add_target( utestData TYPE LIBRARY PCH pchData WARNINGS_AS_ERRORS ON )

target_link_libraries(utestData PUBLIC 
                      core
                      data
                      utest
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
