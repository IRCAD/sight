fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwGui
    fwRender
    fwRuntime
    fwServices
    fwTools
    fwVtkIO
)

find_package(VTK REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

if(NOT ANDROID)
    fwLink(
        vtkInteractionWidgets
        vtkInteractionStyle
        vtkRenderingFreeTypeOpenGL
        vtkRenderingVolumeOpenGL
    )
else()
    fwLink(
        vtkRenderingOpenGL2
        vtkInteractionStyle
    )
endif()
