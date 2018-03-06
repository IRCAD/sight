fwLoadProperties()

find_package(Eigen REQUIRED)
find_package(OpenCV REQUIRED)

fwForwardInclude(
    ${EIGEN3_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_calib3d
)
