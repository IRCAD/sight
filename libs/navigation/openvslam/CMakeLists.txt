sight_target( navigation_openvslam TYPE LIBRARY PCH OFF )


# For now openvslam is only available for unix platforms.
if(UNIX)
    find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_calib3d )

    find_package(OpenMP REQUIRED)
    find_package(openvslam QUIET REQUIRED COMPONENTS openvslam)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC openvslam::openvslam)
endif()
target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
)
