fwLoadProperties()

if(ANDROID)
find_package(VTK COMPONENTS     
        vtkRenderingOpenGL2
        vtkInteractionStyle
        REQUIRED
        NO_MODULE)
else()
find_package(VTK COMPONENTS       
        vtkRenderingOpenGL
        vtkRenderingFreeTypeOpenGL
        vtkInteractionWidgets
        REQUIRED
        NO_MODULE)

endif()

include(${VTK_USE_FILE})

fwForwardInclude( ${VTK_INCLUDE_DIRS} )
fwLink( ${VTK_LIBRARIES} )

