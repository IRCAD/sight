sight_add_target(activityTest TYPE TEST)

target_link_libraries(activityTest PUBLIC core data activity utest_data)

add_dependencies(activityTest module_service)
