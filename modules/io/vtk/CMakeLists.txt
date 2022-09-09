sight_add_target(module_io_vtk TYPE MODULE)

find_package(VTK QUIET COMPONENTS vtkIOImport REQUIRED)

target_link_libraries(module_io_vtk PUBLIC VTK::IOImport)

add_dependencies(module_io_vtk data module_service)

target_link_libraries(module_io_vtk PUBLIC core data io_base ui_base service io_vtk)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
