fwLoadProperties()

find_package(OpenCV QUIET REQUIRED core aruco)
find_package(FFMPEG QUIET REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)
fwLink(
    ${OpenCV_LIBS}
    ${FFMPEG_LIBRARIES}
)

