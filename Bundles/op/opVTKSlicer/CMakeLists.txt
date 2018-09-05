fwLoadProperties()

find_package(VTK QUIET COMPONENTS
    vtkCommonCore
    vtkImagingCore
    REQUIRED
    NO_MODULE)

include(${VTK_USE_FILE})

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)

fwForwardLink(
    ${VTK_LIBRARIES}
)
