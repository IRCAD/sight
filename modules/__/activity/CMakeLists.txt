sight_add_target(module_activity TYPE MODULE)

add_dependencies(module_activity data module_service)

target_link_libraries(module_activity PUBLIC core data ui service)
