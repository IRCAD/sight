sight_add_target(activity_ut TYPE TEST)

target_link_libraries(activity_ut PUBLIC core data activity utest_data)

add_dependencies(activity_ut module_service)
