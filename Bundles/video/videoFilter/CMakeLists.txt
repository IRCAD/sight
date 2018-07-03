fwLoadProperties()

find_package(OpenCV QUIET COMPONENTS opencv_core opencv_imgproc REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    ${OpenCV_LIBS}
)

