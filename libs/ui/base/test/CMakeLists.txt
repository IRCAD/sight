sight_add_target(ui_base_test TYPE TEST)

add_dependencies(ui_base_test module_service module_ui_base)

target_link_libraries(ui_base_test PUBLIC core data ui_base io_zip utest)
