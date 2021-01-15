fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkFiltersTexture
    vtkImagingGeneral
    REQUIRED
)

if(VCPKG_TARGET_TRIPLET)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE 
        VTK::FiltersTexture    
        VTK::ImagingGeneral
    )
else()
    fwInclude(
        ${VTK_INCLUDE_DIRS}
    )
    fwLink(
        vtkFiltersTexture
        vtkImagingGeneral
    )
endif()

message(DEPRECATION "The VTK renderer is deprecated and will be removed in sight 22.0.
                     Please switch to the Ogre renderer.")