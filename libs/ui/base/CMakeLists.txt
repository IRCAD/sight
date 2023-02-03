sight_add_target(ui_base TYPE LIBRARY)

target_link_libraries(ui_base PUBLIC activity core data service io_zip)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
