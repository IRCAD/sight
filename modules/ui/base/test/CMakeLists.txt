sight_add_target(module_ui_baseTest TYPE TEST)

add_dependencies(module_ui_baseTest module_ui_base)

target_link_libraries(module_ui_baseTest PUBLIC core service CppUnit)

target_link_libraries(module_ui_baseTest PUBLIC ui_base)
