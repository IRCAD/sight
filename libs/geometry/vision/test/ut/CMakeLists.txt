sight_add_target(geometry_vision_ut TYPE TEST)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_imgcodecs opencv_highgui)
target_link_libraries(geometry_vision_ut PUBLIC opencv_highgui PRIVATE opencv_imgcodecs)

target_link_libraries(geometry_vision_ut PRIVATE "$<$<VERSION_GREATER_EQUAL:glm_VERSION,0.9.9.8>:glm::glm>")

target_link_libraries(geometry_vision_ut PUBLIC core data utest_data geometry_data geometry_vision)
