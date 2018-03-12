fwLoadProperties()

find_package(OpenCV QUIET REQUIRED)
find_package(Boost QUIET COMPONENTS regex REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_videoio
    opencv_imgcodecs
    opencv_imgproc
    ${Boost_REGEX_LIBRARY}
)

