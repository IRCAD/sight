fwLoadProperties()

find_package(VTK REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

if(ANDROID)
    fwLink(
        vtkRenderingOpenGL2
        vtkFiltersSources
        vtkInteractionStyle
    )
else()
    fwLink(
        vtkInteractionWidgets
        vtkInteractionStyle
    )
endif()
