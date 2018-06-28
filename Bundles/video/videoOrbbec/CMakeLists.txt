fwLoadProperties()

find_package(OpenNI2 QUIET REQUIRED)
find_package(Qt5 QUIET COMPONENTS Widgets Multimedia REQUIRED)

fwInclude(
    ${OPENNI2_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Multimedia_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)
fwLink(
    ${OPENNI2_LIBRARY}
    ${Qt5Core_LIBRARIES}
    ${Qt5Multimedia_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

if(WIN32)
    if(OPENNI2_FOUND)
        install(DIRECTORY ${OPENNI2_DRIVERS_DIR} DESTINATION "bin")
    endif()
else()
    if(OPENNI2_FOUND)
        install(DIRECTORY ${OPENNI2_DRIVERS_DIR} DESTINATION "lib")
    endif()
endif()
