fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_core opencv_imgcodecs)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_core opencv_imgcodecs)
