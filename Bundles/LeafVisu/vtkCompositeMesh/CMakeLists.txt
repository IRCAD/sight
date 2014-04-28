fwLoadProperties()
fwUseForwardInclude(
    fwData
    fwRender
    fwRenderVTK
    fwRuntime
    fwServices
)

find_package(Boost REQUIRED)
find_package(VTK COMPONENTS  REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${VTK_INCLUDE_DIRS}
    )
fwLink(
    ${Boost_LIBRARIES}
    vtkInteractionWidgets
    )
