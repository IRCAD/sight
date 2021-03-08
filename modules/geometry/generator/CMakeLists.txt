sight_add_target( module_geometry_generator TYPE MODULE )


find_package(VTK QUIET COMPONENTS vtkFiltersCore REQUIRED)

if(VCPKG_TARGET_TRIPLET)
    # VTK9
    target_link_libraries(module_geometry_generator PRIVATE 
        VTK::vtkFiltersCore
    )

else()
    include(${VTK_USE_FILE})
    target_link_libraries(module_geometry_generator PRIVATE
    vtkFiltersCore
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
