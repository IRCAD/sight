fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkFiltersGeometry
    vtkFiltersSources
    vtkImagingCore
    vtkInteractionStyle
    vtkInteractionWidgets
    vtkRenderingAnnotation
    vtkRenderingFreeType
    vtkRenderingOpenGL2
    REQUIRED)

#add vtk definitions instead of include(${VTK_USE_FILE})
target_compile_definitions(${FWPROJECT_NAME} PUBLIC ${VTK_DEFINITIONS})

fwInclude(
    ${VTK_INCLUDE_DIRS}
)
fwForwardLink(
    vtkFiltersGeometry
    vtkFiltersSources
    vtkImagingCore
    vtkInteractionStyle
    vtkInteractionWidgets
    vtkRenderingAnnotation
    vtkRenderingFreeType
    vtkRenderingOpenGL2
)
