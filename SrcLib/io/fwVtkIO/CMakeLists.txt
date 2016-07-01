fwLoadProperties()

if(NOT ANDROID)
    find_package(VTK COMPONENTS     
        vtkFiltersModeling
        vtkIOImage
        vtkIOLegacy
        vtkIOXML
        vtkImagingStencil
        vtkImagingStatistics
        vtkFiltersGeometry
        vtkInteractionStyle
        vtkIOExport
        vtkRenderingFreeTypeOpenGL
        REQUIRED
        NO_MODULE)
else()
    find_package(VTK COMPONENTS     
        vtkFiltersModeling
        vtkIOImage
        vtkIOLegacy
        vtkIOXML
        vtkImagingStencil
        vtkImagingStatistics
        vtkFiltersGeometry
        vtkInteractionStyle
        vtkRenderingOpenGL2
        REQUIRED
        NO_MODULE)
endif()

include(${VTK_USE_FILE})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    ${VTK_LIBRARIES}
)
