# For now openvslam is only available for unix platforms.
if(UNIX)
    fwLoadProperties()
    
    find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d)

    # OpenMP is required for openvslam
    find_package(OpenMP REQUIRED)
    find_package(openvslam QUIET REQUIRED COMPONENTS openvslam)

    target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_calib3d openvslam::openvslam)

endif()
