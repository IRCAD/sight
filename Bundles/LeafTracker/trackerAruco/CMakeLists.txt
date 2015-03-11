link_directories(${CMAKE_PREFIX_PATH}/lib)

fwLoadProperties()
fwUseForwardInclude(
    fwRuntime
    fwCore
    fwData
    fwDataTools
    fwServices
    fwCom
    fwComEd
    fwTools
    fwMath
    arData
)

find_package(VTK REQUIRED)
find_package(OpenCV REQUIRED) #used by aruco
find_package(aruco REQUIRED)

fwLink(
    ${aruco_LIBS}
    vtkCommonDataModel
    vtkFiltersCore
    vtkFiltersGeometry
    vtkFiltersGeneral
)
fwInclude(
    ${VTK_INCLUDE_DIRS}
)

