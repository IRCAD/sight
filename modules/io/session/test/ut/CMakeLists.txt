sight_add_target(module_io_session_ut TYPE TEST)

add_dependencies(module_io_session_ut data module_service module_ui module_io_session)

target_link_libraries(module_io_session_ut PUBLIC utest_data data service ui io_session)
