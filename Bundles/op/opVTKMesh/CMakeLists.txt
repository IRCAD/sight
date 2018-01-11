fwLoadProperties()

find_package(VTK COMPONENTS vtkInteractionWidgets NO_MODULE)

include(${VTK_USE_FILE})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    ${VTK_LIBRARIES}
)

