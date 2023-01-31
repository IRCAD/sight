sight_add_target(module_io_bitmap TYPE MODULE)

add_dependencies(module_io_bitmap module_service)

target_link_libraries(module_io_bitmap PUBLIC core data io_base ui_base service io_bitmap)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
