fwLoadProperties()

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
include(${VTK_USE_FILE})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    ${VTK_LIBRARIES} 
)
