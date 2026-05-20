sight_add_target(module_io_dimse_ut TYPE DOCTEST)

add_dependencies(module_io_dimse_ut data module_service module_ui module_io_dimse)

target_link_libraries(module_io_dimse_ut PRIVATE core service io_dimse utest_data)
