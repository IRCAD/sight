fwLoadProperties()

if(NOT ANDROID)
    find_package(VTK COMPONENTS vtkIOImport REQUIRED NO_MODULE)
    include(${VTK_USE_FILE})
endif()

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    ${VTK_LIBRARIES}
)
