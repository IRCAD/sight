fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkFiltersGeometry
    vtkFiltersModeling
    vtkIOExport
    vtkIOGeometry
    vtkIOImage
    vtkIOLegacy
    vtkIOPLY
    vtkIOXML
    vtkRenderingFreeType
    vtkRenderingOpenGL2  # Needed notably for the VtkObjExporter
    REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
    VTK::FiltersGeometry
    VTK::FiltersModeling
    VTK::IOExport
    VTK::IOGeometry
    VTK::IOImage
    VTK::IOLegacy
    VTK::IOPLY
    VTK::IOXML
    VTK::RenderingFreeType
    VTK::RenderingOpenGL2 
)

#add vtk definitions instead of include(${VTK_USE_FILE})
target_compile_definitions(${FWPROJECT_NAME} PRIVATE ${VTK_DEFINITIONS})

