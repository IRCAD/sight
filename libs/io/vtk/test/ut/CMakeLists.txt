sight_add_target(io_vtk_ut TYPE TEST)

find_package(VTK QUIET COMPONENTS vtkFiltersSources REQUIRED)

target_link_libraries(io_vtk_ut PRIVATE VTK::FiltersSources)

target_link_libraries(io_vtk_ut PUBLIC core utest_data data io_vtk)
