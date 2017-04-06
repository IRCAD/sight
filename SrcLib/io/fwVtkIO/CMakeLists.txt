fwLoadProperties()

if(NOT ANDROID)
    find_package(VTK COMPONENTS
        vtkFiltersModeling
        vtkIOImage
        vtkIOLegacy
        vtkIOXML
        vtkFiltersGeometry
        vtkIOExport
        REQUIRED
        NO_MODULE)
else()
    find_package(VTK COMPONENTS
        vtkFiltersModeling
        vtkIOImage
        vtkIOLegacy
        vtkIOXML
        vtkFiltersGeometry
        vtkInteractionStyle
        vtkRenderingOpenGL2
        vtkRenderingFreeType
        REQUIRED
        NO_MODULE)
endif()

include(${VTK_USE_FILE})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwForwardLink(
    ${VTK_LIBRARIES}
)
