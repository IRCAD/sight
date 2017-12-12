fwLoadProperties()

find_package(OpenCV COMPONENTS core  REQUIRED)
find_package(Eigen REQUIRED)
find_package(Ceres REQUIRED)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${CERES_INCLUDE_DIR}
)

fwLink(
    ${CERES_LIBRARIES}
    opencv_core
)
