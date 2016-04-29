fwLoadProperties()

if(NOT ANDROID)
    find_package(VTK
        COMPONENTS
        vtkInteractionWidgets
        vtkInteractionStyle
        vtkFiltersTexture
        vtkRenderingOpenGL
        vtkIOLegacy
        REQUIRED
    )
else()
    find_package(VTK
        COMPONENTS
        vtkInteractionStyle
        vtkFiltersTexture
        vtkRenderingOpenGL2
        vtkIOLegacy
        REQUIRED
    )
endif()

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    ${VTK_LIBRARIES}
)

