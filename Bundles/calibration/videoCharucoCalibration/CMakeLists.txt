fwLoadProperties()

find_package(OpenCV QUIET REQUIRED core aruco)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)
fwLink(
    ${OpenCV_LIBS}
)

