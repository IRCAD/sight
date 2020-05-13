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

#add vtk definitions instead of include(${VTK_USE_FILE})
target_compile_definitions(${FWPROJECT_NAME} PRIVATE ${VTK_DEFINITIONS})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwForwardLink(
    vtkFiltersGeometry
    vtkFiltersModeling
    vtkIOExport
    vtkIOGeometry
    vtkIOImage
    vtkIOLegacy
    vtkIOPLY
    vtkIOXML
    vtkRenderingFreeType
    vtkRenderingOpenGL2
)

if(UNIX)
    # Necessary for all related unit-tests and utilities
    find_package(GLEW)
    fwForwardLink(
        ${GLEW_LIBRARIES}
    )
endif()
