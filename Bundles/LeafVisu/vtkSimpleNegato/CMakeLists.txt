fwLoadProperties()

find_package(VTK COMPONENTS vtkInteractionWidgets REQUIRED NO_MODULE)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS} 
)
fwLink(
    ${VTK_LIBRARIES}
    
)

