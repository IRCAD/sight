sight_add_target(module_ui TYPE MODULE)

add_dependencies(module_ui module_service)

target_link_libraries(module_ui PUBLIC core data service activity io ui)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif()
