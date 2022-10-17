sight_add_target(module_navigation_openvslam TYPE MODULE PCH OFF)

find_package(
    OpenCV QUIET REQUIRED COMPONENTS core highgui imgproc imgcodecs OPTIONAL_COMPONENTS cudawarping cudaimgproc
)
target_link_libraries(module_navigation_openvslam PRIVATE opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs)

find_package(Boost QUIET REQUIRED COMPONENTS serialization)
target_link_libraries(module_navigation_openvslam PRIVATE Boost::serialization)

find_package(openvslam QUIET COMPONENTS openvslam REQUIRED)
target_link_libraries(module_navigation_openvslam PRIVATE openvslam::openvslam)

# FIXME: headers of DBoW2.
find_package(DBoW2 REQUIRED)
target_include_directories(module_navigation_openvslam PRIVATE ${DBoW2_INCLUDE_DIRS})
target_link_libraries(module_navigation_openvslam PRIVATE ${DBoW2_LIBS})

find_package(Eigen3 QUIET REQUIRED)
target_link_libraries(module_navigation_openvslam PRIVATE Eigen3::Eigen)

target_link_libraries(
    module_navigation_openvslam
    PUBLIC core
           io_http
           data
           service
           ui_base
           io_opencv
           navigation_openvslam
)
