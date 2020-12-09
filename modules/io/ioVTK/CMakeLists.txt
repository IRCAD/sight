fwLoadProperties()

find_package(VTK QUIET COMPONENTS vtkIOImport REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC VTK::IOImport)
