sight_add_target(module_ui_ut TYPE TEST)

add_dependencies(module_ui_ut module_ui)

target_link_libraries(module_ui_ut PUBLIC core service CppUnit)

target_link_libraries(module_ui_ut PUBLIC ui)
