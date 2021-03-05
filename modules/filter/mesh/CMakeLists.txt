sight_add_target( module_filter_mesh TYPE MODULE )


find_package(VTK QUIET COMPONENTS vtkFiltersCore REQUIRED)

if(VCPKG_TARGET_TRIPLET)
    # VTK9
    target_link_libraries(module_filter_mesh PRIVATE 
        VTK::vtkFiltersCore
    )

else()
    include(${VTK_USE_FILE})
    target_link_libraries(module_filter_mesh PRIVATE
    vtkFiltersCore
    )
endif()


target_link_libraries(module_filter_mesh PUBLIC 
                      data
                      ui_base
                      service
                      io_vtk
                      geometry_data
                      core
)
