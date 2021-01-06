fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkFiltersGeometry
    vtkFiltersSources
    vtkImagingCore
    vtkImagingGeneral
    vtkInteractionStyle
    vtkInteractionWidgets
    vtkRenderingAnnotation
    vtkRenderingFreeType
    vtkRenderingOpenGL2
    # Not used in this module, but we include it here to avoid conflicts in PCH with overriding definitions
    vtkRenderingVolumeOpenGL2
    REQUIRED)

if(VCPKG_TARGET_TRIPLET)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC 
        VTK::FiltersGeometry
        VTK::FiltersSources
        VTK::ImagingCore
        VTK::ImagingGeneral
        VTK::InteractionStyle
        VTK::InteractionWidgets
        VTK::RenderingAnnotation
        VTK::RenderingFreeType
        VTK::RenderingOpenGL2 
    )
else()
    fwInclude(
        ${VTK_INCLUDE_DIRS}
    )
    fwForwardLink(
        vtkFiltersGeometry
        vtkFiltersSources
        vtkImagingCore
        vtkImagingGeneral
        vtkInteractionStyle
        vtkInteractionWidgets
        vtkRenderingAnnotation
        vtkRenderingFreeType
        vtkRenderingOpenGL2
        vtkRenderingVolumeOpenGL2
    )
endif()

#add vtk definitions instead of include(${VTK_USE_FILE})
target_compile_definitions(${FWPROJECT_NAME} PUBLIC ${VTK_DEFINITIONS})

message(DEPRECATION "The VTK renderer is deprecated and will be removed in sight 22.0.
                     Please switch to the Ogre renderer.")
