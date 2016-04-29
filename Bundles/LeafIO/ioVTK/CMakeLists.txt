fwLoadProperties()


find_package(VTK REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

if (NOT ANDROID)
    fwLink(
        vtkIOImport
    )
endif()
