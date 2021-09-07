sight_add_target( navigation_openvslamTest TYPE TEST PCH OFF)

# OpenMP is required for openvslam
find_package(OpenMP REQUIRED)
find_package(openvslam QUIET COMPONENTS openvslam REQUIRED)
target_link_libraries(navigation_openvslamTest PUBLIC openvslam::openvslam)

target_link_libraries(navigation_openvslamTest PUBLIC utestData navigation_openvslam)
