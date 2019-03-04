fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS imgproc)

target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_imgproc)

