fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwData
    fwRenderVTK
    fwRuntime
)

find_package(VTK 
    COMPONENTS
    vtkRenderingVolume
    vtkInteractionWidgets
    vtkRenderingVolumeOpenGL
    vtkRenderingCore
    vtkIOImage
    vtkCommonDataModel
    vtkCommonCore
    REQUIRED
)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    ${VTK_LIBRARIES} 
)
