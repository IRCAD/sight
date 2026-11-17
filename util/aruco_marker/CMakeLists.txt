sight_add_target(aruco_marker TYPE EXECUTABLE CONSOLE ON)

find_package(Boost QUIET REQUIRED COMPONENTS program_options)
target_link_libraries(aruco_marker PRIVATE Boost::program_options)

find_package(OpenCV REQUIRED COMPONENTS core)

if(OpenCV_VERSION VERSION_LESS "5.0")
    find_package(OpenCV REQUIRED COMPONENTS aruco highgui)
    target_link_libraries(aruco_marker PRIVATE opencv_aruco opencv_highgui)
else()
    find_package(OpenCV REQUIRED COMPONENTS objdetect highgui)
    target_link_libraries(aruco_marker PRIVATE opencv_objdetect opencv_highgui)
endif()
