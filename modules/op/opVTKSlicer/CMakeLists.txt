fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkImagingCore
    REQUIRED
)

if(VCPKG_TARGET_TRIPLET)
    # VTK9
    target_link_libraries(${FWPROJECT_NAME} PUBLIC 
        VTK::ImagingCore
    )

else()
    include(${VTK_USE_FILE})
    target_link_libraries(${FWPROJECT_NAME} PUBLIC
        vtkImagingCore
    )
endif()
