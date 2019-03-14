fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkFiltersModeling
    vtkIOImage
    vtkIOLegacy
    vtkIOXML
    vtkFiltersGeometry
    vtkIOExport
    vtkRenderingFreeType
    vtkRenderingOpenGL2  # Needed notably for the VtkObjExporter
    REQUIRED)

#add vtk definitions instead of include(${VTK_USE_FILE})
target_compile_definitions(${FWPROJECT_NAME} PUBLIC ${VTK_DEFINITIONS})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwForwardLink(
    vtkFiltersModeling
    vtkIOImage
    vtkIOLegacy
    vtkIOXML
    vtkFiltersGeometry
    vtkIOExport
    vtkRenderingFreeType
    vtkRenderingOpenGL2
)
