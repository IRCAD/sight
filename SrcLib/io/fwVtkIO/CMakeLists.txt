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
)
    
if(NOT ANDROID)
    fwLink(
        vtkIOExport
    )
endif()
