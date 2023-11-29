sight_add_target(aruco_marker TYPE EXECUTABLE CONSOLE ON)

find_package(Boost QUIET REQUIRED COMPONENTS program_options)
target_link_libraries(aruco_marker PRIVATE Boost::program_options)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_aruco opencv_highgui)
target_link_libraries(aruco_marker PRIVATE opencv_aruco opencv_highgui)
