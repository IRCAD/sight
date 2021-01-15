fwLoadProperties()


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_core opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_core opencv_highgui)