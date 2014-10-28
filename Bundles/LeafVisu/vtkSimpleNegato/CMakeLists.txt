fwLoadProperties()
fwUseForwardInclude(
    fwRender
    fwRenderVTK
    fwRuntime
)

find_package(Boost REQUIRED)
find_package(VTK REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${VTK_INCLUDE_DIRS} 
    )
fwLink(
    vtkWidgets
    )

