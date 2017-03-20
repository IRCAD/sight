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
            vtkRenderingCore
            vtkRenderingAnnotation
            vtkRenderingFreeType
            vtkRenderingVolumeOpenGL2
        REQUIRED
        NO_MODULE)
    else()
        find_package(VTK COMPONENTS
            vtkInteractionWidgets
            vtkInteractionStyle
            vtkRenderingCore
            vtkRenderingAnnotation
            vtkRenderingFreeTypeOpenGL
            vtkRenderingVolumeOpenGL
        REQUIRED
        NO_MODULE)
    endif()
else()
    find_package(VTK COMPONENTS
            vtkRenderingOpenGL2
            vtkInteractionStyle
            vtkInteractionWidgets
            REQUIRED
            NO_MODULE)
endif()

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwForwardLink(
    ${VTK_LIBRARIES}
)
