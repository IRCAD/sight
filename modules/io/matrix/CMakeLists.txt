sight_add_target(module_io_matrix TYPE MODULE)

target_link_libraries(module_io_matrix PUBLIC core data io ui service geometry_data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
