fwLoadProperties()

if(NOT ANDROID)
    find_package(VTK COMPONENTS
            vtkInteractionWidgets
            vtkInteractionStyle
            vtkRenderingCore
            vtkRenderingFreeTypeOpenGL
            vtkRenderingVolumeOpenGL
            vtkRenderingAnnotation
            REQUIRED
            NO_MODULE)
else()
    find_package(VTK COMPONENTS
            vtkRenderingOpenGL2
            vtkInteractionStyle
            REQUIRED
            NO_MODULE)
endif()

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    ${VTK_LIBRARIES}
)
