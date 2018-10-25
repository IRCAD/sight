fwLoadProperties()

find_package(Boost QUIET COMPONENTS program_options filesystem REQUIRED)
find_package(OpenCV QUIET REQUIRED imgcodecs)
find_package(Qt5 QUIET COMPONENTS Test REQUIRED)

fwLink(
    Boost::filesystem
    Boost::program_options
    opencv_imgcodecs
    Qt5::Test
)
