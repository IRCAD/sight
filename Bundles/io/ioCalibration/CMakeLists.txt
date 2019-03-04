fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS core)

target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_core)
