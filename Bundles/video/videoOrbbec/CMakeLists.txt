fwLoadProperties()

find_package(OpenNI2 QUIET REQUIRED)
find_package(Boost QUIET COMPONENTS filesystem REQUIRED)
find_package(OpenCV QUIET COMPONENTS videoio )
find_package(FFMPEG QUIET REQUIRED)

fwInclude(
    ${OPENNI2_INCLUDE_DIRS}
    ${Boost_FILESYSTEM_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}

)
fwLink(
    ${OPENNI2_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${OpenCV_LIBS}
    ${FFMPEG_LIBRARIES}

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
