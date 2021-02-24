sight_add_target( io_vtkTest TYPE TEST )


find_package(VTK QUIET COMPONENTS vtkFiltersSources REQUIRED)

if(WIN32)
    # VTK9
    target_link_libraries(${FWPROJECT_NAME} PRIVATE VTK::FiltersSources)
else()
    # VTK7
    fwInclude(
        ${VTK_INCLUDE_DIRS}
    )
    fwLink(
        vtkFiltersSources
    )

    if(UNIX)
        find_package(GLEW)
        fwLink(
            ${GLEW_LIBRARIES}
        )
    endif()
endif()

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
                      io_vtk
)
