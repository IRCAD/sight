sight_add_target(module_io_itkTest TYPE TEST)

add_dependencies(module_io_itkTest data module_service module_ui module_io_itk)

target_link_libraries(module_io_itkTest PUBLIC core utest_data data service io)
