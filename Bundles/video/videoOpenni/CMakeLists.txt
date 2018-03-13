fwLoadProperties()

find_package(OpenNI2 QUIET REQUIRED)

fwInclude(
    ${OPENNI2_INCLUDE_DIRS}
)
fwLink(
    ${OPENNI2_LIBRARY}
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
