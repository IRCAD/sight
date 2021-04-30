sight_add_target( module_filter_image TYPE MODULE )


# Necessary only because of SPlaneSlicer, can be removed if rewritten as a visualisation adaptor
find_package(VTK QUIET COMPONENTS
    vtkImagingCore
    REQUIRED
)

if(VCPKG_TARGET_TRIPLET)
    # VTK9
    target_link_libraries(module_filter_image PUBLIC 
        VTK::ImagingCore
    )

else()
    include(${VTK_USE_FILE})
    target_link_libraries(module_filter_image PUBLIC
        vtkImagingCore
    )
endif()
target_link_libraries(module_filter_image PUBLIC 
                      data
                      service
                      filter_image
                      io_vtk
                      ui_base
                      ui_history
)
