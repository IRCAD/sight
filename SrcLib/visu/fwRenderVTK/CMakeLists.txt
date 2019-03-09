fwLoadProperties()

find_package(VTK QUIET REQUIRED)

#add vtk definitions instead of include(${VTK_USE_FILE})
target_compile_definitions(${FWPROJECT_NAME} PUBLIC ${VTK_DEFINITIONS})

fwInclude(
    ${VTK_INCLUDE_DIRS}
)
fwForwardLink(
    ${VTK_LIBRARIES}
)
