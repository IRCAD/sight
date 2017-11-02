fwLoadProperties()

find_package(OpenCV REQUIRED)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwForwardLink(
    opencv_core
)
