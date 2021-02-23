sight_target( arucoMarker TYPE EXECUTABLE CONSOLE ON )


find_package(Boost QUIET REQUIRED COMPONENTS program_options)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::program_options)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_aruco opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_aruco opencv_highgui)

