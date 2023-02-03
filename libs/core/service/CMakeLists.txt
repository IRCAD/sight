sight_add_target(service TYPE LIBRARY)

target_link_libraries(service PUBLIC core data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
