fwLoadProperties()

find_package(OpenCV QUIET REQUIRED)
find_package(FFMPEG QUIET REQUIRED)
find_package(Boost QUIET COMPONENTS filesystem REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_videoio
    opencv_imgcodecs
    opencv_imgproc
    ${Boost_FILESYSTEM_LIBRARY}
     ${FFMPEG_LIBRARIES}
)

