sight_add_target(app_ut TYPE TEST)

add_dependencies(app_ut module_app)

target_link_libraries(app_ut PUBLIC core utest_data app)
