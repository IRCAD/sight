fwLoadProperties()

find_package(OpenCV REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_videoio
    opencv_imgcodecs
    opencv_imgproc
)

