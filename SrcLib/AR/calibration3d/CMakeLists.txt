fwLoadProperties()

find_package(OpenCV REQUIRED)
find_package(Ceres REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${CERES_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_calib3d
    ${CERES_LIBRARIES}
)
