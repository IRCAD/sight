fwLoadProperties()

find_package(OpenCV REQUIRED core aruco)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwForwardLink(
    ${OpenCV_LIBS}
)
