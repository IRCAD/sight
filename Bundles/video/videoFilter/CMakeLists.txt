fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_imgproc)

target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_imgproc)

