link_directories(${CMAKE_PREFIX_PATH}/lib)

fwLoadProperties()

find_package(OpenCV REQUIRED) #used by aruco
find_package(aruco REQUIRED)

fwLink(
    ${aruco_LIBS}
)

