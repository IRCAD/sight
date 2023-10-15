sight_add_target(appTest TYPE TEST)

add_dependencies(appTest module_app)

target_link_libraries(appTest PUBLIC core utest_data app)
