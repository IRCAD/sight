fwLoadProperties()

find_package(OpenCV QUIET REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_calib3d
)
