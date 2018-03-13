fwLoadProperties()

find_package(OpenCV QUIET REQUIRED)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwForwardLink(
    opencv_core
)
