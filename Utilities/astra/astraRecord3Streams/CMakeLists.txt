fwLoadProperties()

find_package(OpenNI2 QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${OPENNI2_LIBRARIES})

find_package(Boost QUIET REQUIRED COMPONENTS program_options)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::program_options)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_videoio opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_videoio opencv_highgui)

find_package(FFMPEG QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})

find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Test)

fwInclude(
    ${OPENNI2_INCLUDE_DIRS}
)

if(WIN32)
    if(OPENNI2_FOUND)
        install(DIRECTORY ${OPENNI2_DRIVERS_DIR} DESTINATION "bin")
    endif()
else()
    if(OPENNI2_FOUND)
        install(DIRECTORY ${OPENNI2_DRIVERS_DIR} DESTINATION "lib")
    endif()
endif()
