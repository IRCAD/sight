link_directories(${CMAKE_PREFIX_PATH}/lib)

fwLoadProperties()

find_package(VTK
            COMPONENTS
            vtkCommonDataModel
            vtkFiltersCore
            vtkFiltersGeometry
            vtkFiltersGeneral
            NO_MODULE
            REQUIRED)
include(${VTK_USE_FILE})

find_package(OpenCV REQUIRED) #used by aruco
find_package(aruco REQUIRED)

fwLink(
    ${aruco_LIBS}
    ${VTK_LIBRARIES}
)
fwInclude(
    ${VTK_INCLUDE_DIRS}
)

