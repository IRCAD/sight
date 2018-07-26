fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Multimedia MultimediaWidgets REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Multimedia_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES} 
    ${Qt5Multimedia_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

