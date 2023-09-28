sight_add_target(ui TYPE LIBRARY)

target_link_libraries(ui PUBLIC activity core data service io_zip)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
