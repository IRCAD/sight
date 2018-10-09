fwLoadProperties()

find_package (Boost QUIET COMPONENTS program_options REQUIRED)
find_package(OpenCV QUIET REQUIRED aruco)
find_package(Qt5 QUIET COMPONENTS Test REQUIRED)

fwLink(
    Boost::program_options
    opencv_aruco
    Qt5::Test
)
