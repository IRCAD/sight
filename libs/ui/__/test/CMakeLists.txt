sight_add_target(ui_test TYPE TEST)

add_dependencies(ui_test module_service module_ui)

target_link_libraries(ui_test PUBLIC core data ui io_zip utest)
