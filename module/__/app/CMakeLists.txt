sight_add_target(module_app TYPE MODULE)

add_dependencies(module_app module_service)

target_link_libraries(module_app PUBLIC app)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/app)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
