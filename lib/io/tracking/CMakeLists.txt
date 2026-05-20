sight_add_target(io_tracking TYPE LIBRARY)

target_link_libraries(io_tracking PUBLIC service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
