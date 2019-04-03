fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkRenderingVolumeOpenGL2
    REQUIRED)

fwInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    vtkRenderingVolumeOpenGL2
)
