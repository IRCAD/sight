sight_add_target(module_app_ut TYPE TEST)

add_dependencies(module_app_ut data module_service app_test_app)

get_target_property(APP_TEST_APP_VERSION app_test_app SIGHT_PROJECT_VERSION)
target_compile_definitions(module_app_ut PRIVATE PROFILE_VERSION="${APP_TEST_APP_VERSION}")

target_link_libraries(module_app_ut PUBLIC app utest)
