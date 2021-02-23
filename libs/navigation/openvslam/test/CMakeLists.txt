sight_target( navigation_openvslamTest TYPE TEST )

# For now openvslam is only available for unix platforms.
if(UNIX)

    # OpenMP is required for openvslam
    find_package(OpenMP REQUIRED)
    find_package(openvslam QUIET COMPONENTS openvslam REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE openvslam::openvslam)
endif()

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      utestData
                      navigation_openvslam
)
