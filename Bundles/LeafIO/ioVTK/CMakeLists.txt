fwLoadProperties()

if(NOT ANDROID)
    find_package(VTK COMPONENTS vtkIOImport REQUIRED NO_MODULE)
endif()

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    ${VTK_LIBRARIES}
)
