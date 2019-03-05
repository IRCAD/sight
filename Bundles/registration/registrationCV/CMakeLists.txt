fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d)

target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_calib3d)
