fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkFiltersTexture
    vtkImagingGeneral
    REQUIRED)

fwInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    vtkFiltersTexture
    vtkImagingGeneral
)

message(DEPRECATION "The VTK renderer is deprecated and will be removed in sight 22.0.
                     Please switch to the Ogre renderer.")