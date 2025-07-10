sight_add_target(io_bitmap_ut TYPE TEST)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_core opencv_imgcodecs opencv_imgproc)
target_link_libraries(io_bitmap_ut PRIVATE opencv_core opencv_imgcodecs opencv_imgproc)

target_link_libraries(io_bitmap_ut PUBLIC utest_data io_bitmap io_dicom io_opencv)
