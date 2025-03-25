sight_add_target(module_io_session TYPE MODULE)

add_dependencies(module_io_session module_service)

target_link_libraries(module_io_session PUBLIC io_session ui service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif()
