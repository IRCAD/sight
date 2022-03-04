sight_add_target(data TYPE LIBRARY)

target_link_libraries(data PUBLIC core)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
