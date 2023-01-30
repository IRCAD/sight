sight_add_target(module_ui_base TYPE MODULE)

add_dependencies(module_ui_base module_service)

target_link_libraries(module_ui_base PUBLIC core data service activity io_base ui_base)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif()
