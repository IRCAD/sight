sight_add_target(io_vtk TYPE LIBRARY)

target_link_libraries(
    io_vtk
    PUBLIC VTK::FiltersGeometry
           VTK::FiltersModeling
           VTK::IOExport
           VTK::IOGeometry
           VTK::IOImage
           VTK::IOLegacy
           VTK::IOPLY
           VTK::IOXML
           VTK::RenderingFreeType
           VTK::RenderingOpenGL2
)

target_link_libraries(io_vtk PUBLIC core data io_base geometry_data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
