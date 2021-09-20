sight_add_target( geometry_vtk TYPE LIBRARY)

find_package(VTK QUIET REQUIRED COMPONENTS
    vtkCommonCore
    vtkFiltersCore
)

if(${VTK_MAJOR_VERSION} VERSION_LESS "8")
    target_include_directories(geometry_vtk SYSTEM PRIVATE ${VTK_INCLUDE_DIRS})
    target_link_libraries(geometry_vtk PRIVATE
        vtkCommonCore
        vtkFiltersCore
    )

    #add vtk definitions instead of include(${VTK_USE_FILE})
    target_compile_definitions(geometry_vtk PRIVATE ${VTK_DEFINITIONS})
else()
    target_link_libraries(geometry_vtk PRIVATE
        VTK::CommonCore
        VTK::FiltersCore
    )
endif()

target_link_libraries(geometry_vtk PUBLIC
                      data
                      core
                      io_vtk
)
if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
