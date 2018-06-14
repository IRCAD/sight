link_directories(${CMAKE_PREFIX_PATH}/lib)

fwLoadProperties()

find_package (Boost QUIET COMPONENTS program_options REQUIRED)
find_package(OpenCV QUIET REQUIRED core aruco)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

fwLink(
    ${OpenCV_LIBS}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)
