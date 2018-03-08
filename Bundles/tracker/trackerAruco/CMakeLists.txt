fwLoadProperties()

find_package(OpenCV QUIET REQUIRED core aruco)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwForwardLink(
    ${OpenCV_LIBS}
)
