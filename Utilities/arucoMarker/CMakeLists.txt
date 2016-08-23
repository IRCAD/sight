link_directories(${CMAKE_PREFIX_PATH}/lib)

fwLoadProperties()

find_package (Boost COMPONENTS program_options REQUIRED)
find_package(OpenCV REQUIRED) #used by aruco
find_package(aruco REQUIRED)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)
fwLink(
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${aruco_LIBS}
)
