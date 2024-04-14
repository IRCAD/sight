add_subdirectory(ex_video_tracking)
add_subdirectory(ex_stereo_ar)

if(SIGHT_ENABLE_OPENVSLAM)
    add_subdirectory(ex_openvslam)
endif()
