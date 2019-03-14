fwLoadProperties()
find_package(VTK QUIET COMPONENTS
    vtkFiltersSources
    REQUIRED)

fwInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    vtkFiltersSources
)
