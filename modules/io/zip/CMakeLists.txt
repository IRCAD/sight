sight_add_target(module_io_zip TYPE MODULE)

target_link_libraries(module_io_zip PUBLIC io_zip io io_session)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif()
