sight_add_target(module_sync_ut TYPE TEST)

add_dependencies(module_sync_ut module_sync)

target_link_libraries(module_sync_ut PUBLIC core service utest)
