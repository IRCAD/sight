fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d)
find_package(Eigen QUIET REQUIRED)

fwInclude(
    ${EIGEN3_INCLUDE_DIR}
)

target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_calib3d)
