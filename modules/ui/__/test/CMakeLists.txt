sight_add_target(module_uiTest TYPE TEST)

add_dependencies(module_uiTest module_ui)

target_link_libraries(module_uiTest PUBLIC core service CppUnit)

target_link_libraries(module_uiTest PUBLIC ui)
