fwLoadProperties()

find_package(OpenCV QUIET REQUIRED)
find_package(Ceres QUIET REQUIRED)
find_package(Qt5 QUIET COMPONENTS Test REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${CERES_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_calib3d
    opencv_aruco
    Qt5::Test
    ${CERES_LIBRARIES}
)
