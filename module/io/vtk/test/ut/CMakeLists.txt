sight_add_target(module_io_vtk_ut TYPE TEST)

add_dependencies(module_io_vtk_ut module_io_vtk)

target_link_libraries(module_io_vtk_ut PUBLIC core utest_data data service ui)
