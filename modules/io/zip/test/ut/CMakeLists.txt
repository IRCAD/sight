sight_add_target(module_io_zip_ut TYPE TEST)

#add_dependencies(module_io_session_ut data module_service module_ui module_io_session)

#target_link_libraries(module_io_session_ut PUBLIC utest_data data service ui io_session)
#add_dependencies(module_io_zip_ut ui io_zip utest_data core)
target_link_libraries(module_io_zip_ut PUBLIC ui ui_test io_vtk utest_data)
