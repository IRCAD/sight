fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwGui
    fwMath
    fwRender
    fwRuntime
    fwServices
    fwTools
)

find_package(VTK REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    vtkInteractionWidgets
    vtkInteractionStyle
    vtkRenderingFreeTypeOpenGL
    vtkRenderingVolumeOpenGL
)
