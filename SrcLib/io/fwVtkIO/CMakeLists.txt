fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwData
    fwDataIO
    fwMedData
    fwTools
    fwJobs
)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    vtkFiltersModeling
    vtkIOImage
    vtkIOLegacy
    vtkIOXML
    vtkImagingStencil
    vtkImagingStatistics
    vtkFiltersGeometry
    vtkInteractionStyle
)
    
if(NOT ANDROID)
    fwLink(
        vtkIOExport
        vtkRenderingFreeTypeOpenGL
    )
else()
    fwLink(
        vtkRenderingOpenGL2
    )
endif()
