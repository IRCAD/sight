fwLoadProperties()

find_package(OpenCV REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_imgproc
)

