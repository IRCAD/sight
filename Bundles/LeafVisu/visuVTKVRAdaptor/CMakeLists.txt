fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwData
    fwRenderVTK
    fwRuntime
)


find_package(Boost REQUIRED)
find_package(VTK COMPONENTS  REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${VTK_INCLUDE_DIRS}
    )
fwLink(
    ${Boost_LIBRARIES}
    vtkRenderingVolume
    )
