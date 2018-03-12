fwLoadProperties()

find_package(OpenCV QUIET REQUIRED)
find_package(Eigen QUIET REQUIRED)

fwForwardInclude(
    ${EIGEN3_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_calib3d
)
