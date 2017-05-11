fwLoadProperties()

# Just look one core module to get the version number
find_package(VTK COMPONENTS
        vtkCommonCore
        REQUIRED
        NO_MODULE)

if(${VTK_MAJOR_VERSION} STREQUAL "7")
    find_package(VTK COMPONENTS
        vtkRenderingOpenGL2
        vtkInteractionStyle
        vtkRenderingFreeType
    REQUIRED
    NO_MODULE)
else()
    find_package(VTK COMPONENTS
        vtkRenderingOpenGL
        vtkInteractionStyle
        vtkRenderingFreeType
    REQUIRED
    NO_MODULE)
endif()

include(${VTK_USE_FILE})

fwForwardInclude( ${VTK_INCLUDE_DIRS} )
fwLink( ${VTK_LIBRARIES} )

