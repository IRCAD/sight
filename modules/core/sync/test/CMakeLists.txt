sight_add_target(module_syncTest TYPE TEST)

add_dependencies(module_syncTest module_sync)

target_link_libraries(module_syncTest PUBLIC core service utest)
