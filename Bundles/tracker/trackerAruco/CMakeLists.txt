link_directories(${CMAKE_PREFIX_PATH}/lib)

fwLoadProperties()

find_package(OpenCV REQUIRED)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwForwardLink(
    opencv_core
    opencv_aruco
)

