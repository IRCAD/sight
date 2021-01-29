fwLoadProperties()

find_package(VTK QUIET COMPONENTS vtkIOImport REQUIRED)

if(WIN32)
    # VTK9
    target_link_libraries(${FWPROJECT_NAME} PUBLIC VTK::IOImport)
else()
    # VTK7
    fwForwardInclude(
        ${VTK_INCLUDE_DIRS}
    )

    fwLink(
        vtkIOImport
    )
endif()
