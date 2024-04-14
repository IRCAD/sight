sight_add_target(module_service TYPE MODULE)

add_dependencies(module_service data)

target_link_libraries(module_service PUBLIC core service)
