sight_add_target(module_filter_mesh TYPE MODULE)

find_package(VTK QUIET REQUIRED COMPONENTS vtkFiltersCore vtkFiltersSources vtkCommonComputationalGeometry)

target_link_libraries(module_filter_mesh PRIVATE VTK::FiltersCore VTK::FiltersSources VTK::CommonComputationalGeometry)

target_link_libraries(module_filter_mesh PUBLIC data service io_vtk geometry_data core)

target_link_libraries(module_filter_mesh PRIVATE "$<$<VERSION_GREATER_EQUAL:glm_VERSION,0.9.9.8>:glm::glm>")

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
