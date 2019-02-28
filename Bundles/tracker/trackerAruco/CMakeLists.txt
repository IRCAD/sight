fwLoadProperties()

find_package(OpenCV QUIET REQUIRED core aruco)
find_package(FFMPEG QUIET REQUIRED)

fwForwardInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)

fwForwardLink(
    ${OpenCV_LIBS}
    ${FFMPEG_LIBRARIES}
)
