sight_add_target( io_vtkTest TYPE TEST )


find_package(VTK QUIET COMPONENTS vtkFiltersSources REQUIRED)

if(${VTK_MAJOR_VERSION} VERSION_LESS "8")
    target_include_directories(io_vtkTest SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})
    target_link_libraries(io_vtkTest PRIVATE vtkFiltersSources)
else()
    target_link_libraries(io_vtkTest PRIVATE VTK::FiltersSources)
endif()

target_link_libraries(io_vtkTest PUBLIC 
                      core
                      utestData
                      data
                      io_vtk
)
