fwLoadProperties()

find_package(VTK QUIET COMPONENTS vtkFiltersSources REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE VTK::FiltersSources)
