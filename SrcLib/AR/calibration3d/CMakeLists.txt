fwLoadProperties()

find_package(OpenCV REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    ${Boost_SYSTEM_LIBRARY}
    opencv_core
    opencv_calib3d
)

