fwLoadProperties()


find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(Qt5 COMPONENTS Core Gui REQUIRED)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})

find_package(VTK REQUIRED)
fwLink(
    vtkCommonCore
    vtkCommonDataModel
    vtkRenderingCore
    vtkCommonComputationalGeometry
    vtkFiltersSources
    )

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
