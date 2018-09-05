fwLoadProperties()

find_package(OpenCV QUIET REQUIRED core calib3d)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwForwardLink(
    ${OpenCV_LIBS}
)
