fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkFiltersSources
    REQUIRED
)

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
