fwLoadProperties()
fwUseForwardInclude(
    fwCom
    fwComEd
    fwData
    fwRenderVTK
    fwRuntime
    fwServices
)

find_package(VTK 
    COMPONENTS
    vtkInteractionWidgets
    vtkInteractionStyle
    vtkFiltersTexture
    vtkRenderingOpenGL
    vtkIOLegacy
    REQUIRED
)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    ${VTK_LIBRARIES} 
    vtkFiltersTexture
)

