fwLoadProperties()

if(ANDROID)
    find_package(VTK
                 COMPONENTS
                 vtkRenderingOpenGL2
                 vtkInteractionStyle
                 NO_MODULE
                 REQUIRED)
else()
    find_package(VTK
                COMPONENTS
                vtkInteractionWidgets
                NO_MODULE
                REQUIRED)
endif()
include(${VTK_USE_FILE})
fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink( ${VTK_LIBRARIES} )
        
