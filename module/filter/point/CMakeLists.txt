sight_add_target(module_filter_point TYPE MODULE)

find_package(VTK QUIET COMPONENTS vtkCommonDataModel REQUIRED)

target_link_libraries(module_filter_point PRIVATE VTK::CommonDataModel)
target_link_libraries(module_filter_point PUBLIC data io_itk ui)
