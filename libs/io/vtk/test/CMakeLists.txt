sight_add_target(io_vtkTest TYPE TEST)

find_package(VTK QUIET COMPONENTS vtkFiltersSources REQUIRED)

target_link_libraries(io_vtkTest PRIVATE VTK::FiltersSources)

target_link_libraries(io_vtkTest PUBLIC core utestData data io_vtk)
