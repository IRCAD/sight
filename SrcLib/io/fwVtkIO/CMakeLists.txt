fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkFiltersModeling
    vtkIOImage
    vtkIOLegacy
    vtkIOXML
    vtkFiltersGeometry
    vtkIOExport
    REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
target_link_libraries(${FWPROJECT_NAME} PUBLIC
    vtkFiltersModeling
    vtkIOImage
    vtkIOLegacy
    vtkIOXML
    vtkFiltersGeometry
    vtkIOExport)

