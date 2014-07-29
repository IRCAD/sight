fwLoadProperties()
fwUseForwardInclude(
    fwGui
    fwRender
    fwRenderVTK
    fwRuntime
    io
)


Find_package (Boost COMPONENTS filesystem REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})


