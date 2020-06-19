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
