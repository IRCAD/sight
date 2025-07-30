sight_add_target(module_data TYPE MODULE)

target_link_libraries(module_data PUBLIC core data service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
