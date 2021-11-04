sight_add_target(io_opencvTest TYPE TEST)

find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
target_link_libraries(io_opencvTest PRIVATE Qt5::Test)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_highgui)
target_link_libraries(io_opencvTest PRIVATE opencv_highgui)

target_link_libraries(io_opencvTest PUBLIC utestData io_opencv)
