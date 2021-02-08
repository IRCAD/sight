fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_core opencv_imgproc opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_core opencv_imgproc opencv_highgui)

# TODO: Add a CMake option
find_package(realsense2 QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${realsense2_LIBRARY})
