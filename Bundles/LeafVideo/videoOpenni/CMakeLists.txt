fwLoadProperties()

find_package (OpenNI2 REQUIRED)

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
endif()
