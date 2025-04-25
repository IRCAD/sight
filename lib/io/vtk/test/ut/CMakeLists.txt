sight_add_target(io_vtk_ut TYPE TEST)

# This is required because of VTK-m double alias definition.
# We should remove this if VTK-m is finally unneeded
if(NOT DEFINED "VTK_FiltersSources_FOUND")
    find_package(VTK QUIET COMPONENTS vtkFiltersSources REQUIRED)
endif()

target_link_libraries(io_vtk_ut PRIVATE VTK::FiltersSources)
target_link_libraries(io_vtk_ut PUBLIC core utest_data data io_vtk)
