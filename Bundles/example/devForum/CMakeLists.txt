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


find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(VTK REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    vtkCommonCore
    vtkRenderingCore
    vtkInteractionWidgets
)


fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(
    ${Boost_FILESYSTEM_LIBRARY}
    ${BOOST_SYSTEM_LIBRARY}
)


