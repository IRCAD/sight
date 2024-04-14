sight_add_target(io_opencv_ut TYPE TEST)

find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
target_link_libraries(io_opencv_ut PRIVATE Qt5::Test)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_highgui)
target_link_libraries(io_opencv_ut PRIVATE opencv_highgui)

target_link_libraries(io_opencv_ut PUBLIC utest_data io_opencv)
