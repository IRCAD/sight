# For now openvslam is only available for unix platforms.
if(UNIX)
    fwLoadProperties()

    find_package(FFMPEG QUIET REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})

    find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Test)
endif()
