sight_add_target(app TYPE LIBRARY)

target_link_libraries(app PUBLIC service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
