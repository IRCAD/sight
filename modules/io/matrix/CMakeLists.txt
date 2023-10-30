sight_add_target(module_io_matrix TYPE MODULE)

target_link_libraries(module_io_matrix PUBLIC core data io ui service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
