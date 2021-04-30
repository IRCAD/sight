sight_add_target( geometry_vtk TYPE LIBRARY )

find_package(VTK QUIET COMPONENTS
    vtkCommonCore
    vtkFiltersCore
    REQUIRED)
    
include(${VTK_USE_FILE})

target_include_directories(geometry_vtk SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})


target_link_libraries(geometry_vtk PUBLIC 
                      data
                      core
                      io_vtk
)
if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
