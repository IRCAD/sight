fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwDataIO
    fwMedData
    fwTools
)

find_package(VTK REQUIRED)

fwForwardInclude(${VTK_INCLUDE_DIRS})
fwLink(vtkFiltersModeling vtkIOImage vtkIOLegacy vtkIOExport vtkIOXML vtkImagingStencil vtkImagingStatistics)

get_target_property(TARGET_TYPE vtkHybrid LOCATION)
