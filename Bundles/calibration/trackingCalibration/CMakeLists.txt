fwLoadProperties()

find_package(OpenCV QUIET REQUIRED core calib3d)
find_package(Eigen QUIET REQUIRED)

fwForwardInclude(
    ${EIGEN3_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    ${OpenCV_LIBS}
)
