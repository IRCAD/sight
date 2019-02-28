fwLoadProperties()

find_package(OpenCV QUIET REQUIRED core calib3d)
find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
find_package(FFMPEG QUIET REQUIRED)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)

fwForwardLink(
    opencv_core
    opencv_calib3d
    Qt5::Test
    ${FFMPEG_LIBRARIES}
)
