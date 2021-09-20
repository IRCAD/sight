sight_add_target( activity TYPE LIBRARY PCH pchData)

target_link_libraries(activity PUBLIC core data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
