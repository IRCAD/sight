fwLoadProperties()

find_package(VTK REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    vtkInteractionWidgets
)

