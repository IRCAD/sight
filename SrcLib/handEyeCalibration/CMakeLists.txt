fwLoadProperties()

find_package(OpenCV QUIET COMPONENTS core  REQUIRED)
find_package(Eigen QUIET REQUIRED)
find_package(Ceres QUIET REQUIRED)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${CERES_INCLUDE_DIR}
)

fwLink(
    ${CERES_LIBRARIES}
    opencv_core
)
