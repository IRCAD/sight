fwLoadProperties()

find_package(OpenCV REQUIRED core aruco)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)
fwLink(
    ${OpenCV_LIBS}
)

