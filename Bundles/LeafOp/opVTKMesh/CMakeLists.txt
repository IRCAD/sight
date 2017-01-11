fwLoadProperties()

find_package(VTK COMPONENTS vtkInteractionWidgets NO_MODULE)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    ${VTK_LIBRARIES}
)

