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
        REQUIRED
        NO_MODULE)
endif()

find_package(GLEW)

include(${VTK_USE_FILE})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwForwardLink(
    ${VTK_LIBRARIES}
    ${GLEW_LIBRARIES}
)
