sight_add_target( module_filter_vision TYPE MODULE )


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_core opencv_imgproc opencv_highgui)
target_link_libraries(module_filter_vision PRIVATE opencv_core opencv_imgproc opencv_highgui)

# TODO: Add a CMake option
find_package(realsense2 QUIET REQUIRED)
target_link_libraries(module_filter_vision PRIVATE ${realsense2_LIBRARY})

target_link_libraries(module_filter_vision PUBLIC 
                      service
                      data
                      geometry_data
                      filter_vision
                      io_opencv
)
