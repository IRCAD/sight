sight_add_target(ui_ut TYPE TEST)

add_dependencies(ui_ut module_service module_ui)

target_link_libraries(ui_ut PUBLIC core data ui io_zip utest)
