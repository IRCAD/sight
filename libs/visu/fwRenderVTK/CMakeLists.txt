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

#add vtk definitions instead of include(${VTK_USE_FILE})
target_compile_definitions(${FWPROJECT_NAME} PUBLIC ${VTK_DEFINITIONS})

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

message(DEPRECATION "The VTK renderer is deprecated and will be removed in sight 22.0.
                     Please switch to the Ogre renderer.")
