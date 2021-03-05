add_subdirectory(ExVideoTracking)
add_subdirectory(ExStereoARCV)

if(SIGHT_ENABLE_OPENVSLAM)
    add_subdirectory(ExOpenvslam)
endif()
