sight_add_target(module_filter_vision TYPE MODULE)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_core opencv_imgproc opencv_highgui opencv_video)
target_link_libraries(module_filter_vision PRIVATE opencv_core opencv_imgproc opencv_highgui opencv_video)

target_link_libraries(module_filter_vision PUBLIC service data geometry_data filter_vision io_opencv)
