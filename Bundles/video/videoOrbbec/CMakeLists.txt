fwLoadProperties()

find_package(Boost QUIET REQUIRED COMPONENTS filesystem)
find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_videoio)
find_package(FFMPEG QUIET REQUIRED)
find_package(OpenNI2 QUIET REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::filesystem)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_videoio)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${OPENNI2_LIBRARIES})

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
