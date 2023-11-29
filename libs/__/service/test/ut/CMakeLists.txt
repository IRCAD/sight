sight_add_target(service_ut TYPE TEST)

add_dependencies(service_ut module_service)

target_link_libraries(service_ut PUBLIC core utest_data data service)
