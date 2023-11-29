sight_add_target(module_io_matrix_ut TYPE TEST)

add_dependencies(module_io_matrix_ut module_io_matrix module_service module_ui)

target_link_libraries(module_io_matrix_ut PUBLIC core utest_data data service ui io_session)
