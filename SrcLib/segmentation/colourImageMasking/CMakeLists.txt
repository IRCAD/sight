fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_ml opencv_imgproc)

target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_ml opencv_imgproc)
