sight_add_target(utest_data TYPE LIBRARY)

target_link_libraries(utest_data PUBLIC core data utest)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
