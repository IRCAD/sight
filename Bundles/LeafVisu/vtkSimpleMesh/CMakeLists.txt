fwLoadProperties()

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS} 
)

if(ANDROID)
    fwLink(
        vtkRenderingOpenGL2
        vtkInteractionStyle
    )
else()
    fwLink(
        vtkRenderingOpenGL
        vtkRenderingFreeTypeOpenGL
        vtkInteractionWidgets
    )
endif()
