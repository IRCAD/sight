fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwData
    fwRender
    fwRenderVTK
    fwRuntime
    fwServices
    fwVtkIO
)

find_package(VTK REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    vtkInteractionWidgets
)
