sight_add_target(ex_settings_uit TYPE GUI_TEST)

add_dependencies(ex_settings_uit ex_settings)

target_link_libraries(ex_settings_uit ui_test ex_settings)
