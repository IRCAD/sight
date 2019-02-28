fwLoadProperties()

find_package(Boost QUIET COMPONENTS program_options filesystem REQUIRED)
find_package(OpenCV QUIET REQUIRED core imgproc imgcodecs)
find_package(FFMPEG QUIET REQUIRED)
find_package(Qt5 QUIET COMPONENTS Test REQUIRED)

fwLink(
    Boost::filesystem
    Boost::program_options
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    Qt5::Test
    ${FFMPEG_LIBRARIES}
)
