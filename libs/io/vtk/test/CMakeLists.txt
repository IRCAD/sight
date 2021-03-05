sight_add_target( io_vtkTest TYPE TEST )


find_package(VTK QUIET COMPONENTS vtkFiltersSources REQUIRED)

if(WIN32)
    # VTK9
    target_link_libraries(io_vtkTest PRIVATE VTK::FiltersSources)
else()
    # VTK7
    target_link_libraries(io_vtkTest PRIVATE vtkFiltersSources)

    if(UNIX)
        find_package(GLEW)
        target_link_libraries(io_vtkTest PRIVATE ${GLEW_LIBRARIES})
    endif()
endif()

target_link_libraries(io_vtkTest PUBLIC 
                      core
                      utestData
                      data
                      io_vtk
)
