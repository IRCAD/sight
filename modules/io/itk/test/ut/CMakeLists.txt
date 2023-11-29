sight_add_target(module_io_itk_ut TYPE TEST)

add_dependencies(module_io_itk_ut data module_service module_ui module_io_itk)

target_link_libraries(module_io_itk_ut PUBLIC core utest_data data service io)
