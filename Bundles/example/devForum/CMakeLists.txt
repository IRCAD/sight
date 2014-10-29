fwLoadProperties()
fwUseForwardInclude(
    fwData
    fwRender
    fwVtkIO
    fwDataIO
    fwComEd
    fwRenderVTK
    io
    gui
)


Find_package (Boost COMPONENTS filesystem REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})


