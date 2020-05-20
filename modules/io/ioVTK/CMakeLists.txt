fwLoadProperties()

find_package(VTK QUIET COMPONENTS vtkIOImport REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwLink(
    vtkIOImport
)
