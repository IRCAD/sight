sight_add_target(activity TYPE LIBRARY)

target_link_libraries(activity PUBLIC core data service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
