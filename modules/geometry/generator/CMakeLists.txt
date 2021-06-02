sight_add_target( module_geometry_generator TYPE MODULE )

find_package(VTK QUIET REQUIRED COMPONENTS
    vtkFiltersCore
    vtkFiltersSources
    vtkCommonComputationalGeometry)

if(${VTK_MAJOR_VERSION} VERSION_LESS "8")
    target_include_directories(module_geometry_generator SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})
    target_link_libraries(module_geometry_generator PRIVATE
        vtkFiltersCore
        vtkFiltersSources
        vtkCommonComputationalGeometry
    )
else()
    target_link_libraries(module_geometry_generator PRIVATE
        VTK::FiltersCore
        VTK::FiltersSources
        VTK::CommonComputationalGeometry
    )
endif()

target_link_libraries(module_geometry_generator PUBLIC 
                      core
                      data
                      ui_base
                      service
                      io_vtk
                      geometry_data
)
