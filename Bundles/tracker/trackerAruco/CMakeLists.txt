fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS aruco)

target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_aruco)
