fwLoadProperties()
fwUseForwardInclude(
    fwCom
    fwData
    fwRender
    fwRenderVTK
    fwRuntime
    fwServices
    fwThread
)

find_package(VTK REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS} 
)
fwLink(
    vtkInteractionWidgets
)

