sight_add_target(service TYPE LIBRARY PCH pchData WARNINGS_AS_ERRORS OFF)

target_link_libraries(service PUBLIC 
                      activity
                      core
                      data
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
