fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS calib3d)

target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_calib3d)
