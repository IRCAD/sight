fwLoadProperties()


find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(VTK
            COMPONENTS
            vtkCommonCore
            vtkRenderingCore
            vtkInteractionWidgets
            NO_MODULE
            REQUIRED)

fwForwardInclude(
    ${VTK_INCLUDE_DIRS}
)
fwLink(
    ${VTK_LIBRARIES}
)


fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(
    ${Boost_FILESYSTEM_LIBRARY}
    ${BOOST_SYSTEM_LIBRARY}
)


