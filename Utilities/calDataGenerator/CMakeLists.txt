fwLoadProperties()

find_package(Boost QUIET COMPONENTS program_options filesystem REQUIRED)
find_package(OpenCV QUIET REQUIRED core imgproc imgcodecs)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    ${OpenCV_LIBS}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)
