fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d)
find_package(Eigen3 QUIET REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Eigen3::Eigen)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_calib3d)
