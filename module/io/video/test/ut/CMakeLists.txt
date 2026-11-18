sight_add_target(module_io_video_ut TYPE TEST)

add_dependencies(module_io_video_ut module_io_video)

target_link_libraries(module_io_video_ut PUBLIC core data service utest PRIVATE opencv_videoio opencv_imgcodecs)
