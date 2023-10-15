sight_add_target(module_io_vtkTest TYPE TEST)

add_dependencies(module_io_vtkTest module_io_vtk)

target_link_libraries(module_io_vtkTest PUBLIC core utest_data data service ui)
