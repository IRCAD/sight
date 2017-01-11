fwLoadProperties()

if(NOT ANDROID)
    # Just look one core module to get the version number
    find_package(VTK COMPONENTS
            vtkCommonCore
            REQUIRED
            NO_MODULE)
            
    if(${VTK_MAJOR_VERSION} STREQUAL "7")
        find_package(VTK COMPONENTS
            vtkInteractionWidgets
            vtkInteractionStyle
            vtkFiltersTexture
            vtkRenderingOpenGL2
            vtkIOLegacy
        REQUIRED
        NO_MODULE)
    else()
        find_package(VTK COMPONENTS
            vtkInteractionWidgets
            vtkInteractionStyle
            vtkFiltersTexture
            vtkRenderingOpenGL
            vtkIOLegacy
        REQUIRED
        NO_MODULE)
    endif()
else()

    find_package(VTK
        COMPONENTS
        vtkInteractionStyle
        vtkFiltersTexture
        vtkRenderingOpenGL2
        vtkIOLegacy
        REQUIRED
        NO_MODULE
    )
endif()

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    ${VTK_LIBRARIES}
)

