sight_add_target(ui_test_app_uit TYPE GUI_TEST)

add_dependencies(ui_test_app_uit ui_test_app)

target_link_libraries(ui_test_app_uit PRIVATE ui_test utest_data)
