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
fwLink(vtkHybrid)

get_target_property(TARGET_TYPE vtkHybrid LOCATION)
