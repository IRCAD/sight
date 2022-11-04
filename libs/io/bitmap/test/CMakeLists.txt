sight_add_target(io_bitmapTest TYPE TEST)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_core opencv_imgcodecs opencv_imgproc)
target_link_libraries(io_bitmapTest PRIVATE opencv_core opencv_imgcodecs opencv_imgproc)

target_link_libraries(io_bitmapTest PUBLIC utestData io_bitmap io_dicom io_opencv)
