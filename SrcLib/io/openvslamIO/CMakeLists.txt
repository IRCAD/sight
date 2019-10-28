fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d)
find_package(openvslam QUIET REQUIRED COMPONENTS openvslam)

target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_calib3d openvslam::openvslam)
