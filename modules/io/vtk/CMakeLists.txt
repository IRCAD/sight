sight_add_target( module_io_vtk TYPE MODULE )

find_package(VTK QUIET COMPONENTS vtkIOImport REQUIRED)

if(WIN32)
    # VTK9
    target_link_libraries(${FWPROJECT_NAME} PUBLIC VTK::IOImport)
else()
    # VTK7
    target_include_directories(${FWPROJECT_NAME} SYSTEM PUBLIC ${VTK_INCLUDE_DIRS})
    target_link_libraries(${FWPROJECT_NAME} PUBLIC vtkIOImport)
endif()

add_dependencies(${FWPROJECT_NAME} data module_service)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      io_base
                      ui_base
                      service
                      io_vtk
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
