fwLoadProperties()

find_package(OpenCV QUIET REQUIRED core calib3d)
find_package(Qt5 QUIET COMPONENTS Test REQUIRED)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwForwardLink(
    opencv_core
    opencv_calib3d
    Qt5::Test
)
