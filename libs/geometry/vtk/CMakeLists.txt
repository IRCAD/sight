sight_add_target( geometry_vtk TYPE LIBRARY )

find_package(VTK QUIET COMPONENTS vtkCommonCore vtkFiltersCore REQUIRED)
    
if(WIN32)
    # VTK9
    target_link_libraries(geometry_vtk PRIVATE VTK::FiltersCore)
else()
    # VTK7
    include(${VTK_USE_FILE})
    target_link_libraries(geometry_vtk PUBLIC vtkFiltersCore)
endif()

target_link_libraries(geometry_vtk PUBLIC data core io_vtk)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
