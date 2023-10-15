sight_add_target(module_io_zipTest TYPE TEST)

#add_dependencies(module_io_sessionTest data module_service module_ui module_io_session)

#target_link_libraries(module_io_sessionTest PUBLIC utest_data data service ui io_session)
#add_dependencies(module_io_zipTest ui io_zip utest_data core)
target_link_libraries(module_io_zipTest PUBLIC ui io_vtk utest_data)
