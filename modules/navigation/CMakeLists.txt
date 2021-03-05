add_subdirectory(calibration)
add_subdirectory(optics)

if(SIGHT_ENABLE_OPENVSLAM)
    add_subdirectory(openvslam)
endif()