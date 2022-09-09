sight_add_target(module_filter_mesh TYPE MODULE)

find_package(VTK QUIET REQUIRED COMPONENTS vtkFiltersCore vtkFiltersSources vtkCommonComputationalGeometry)

target_link_libraries(module_filter_mesh PRIVATE VTK::FiltersCore VTK::FiltersSources VTK::CommonComputationalGeometry)

target_link_libraries(module_filter_mesh PUBLIC data ui_base service io_vtk geometry_data core)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
