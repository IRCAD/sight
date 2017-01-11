fwLoadProperties()

find_package(Qt5 COMPONENTS Core Multimedia MultimediaWidgets REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Multimedia_INCLUDE_DIRS}
    ${Qt5MultimediaWidgets_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES} 
    ${Qt5Multimedia_LIBRARIES} 
    ${Qt5MultimediaWidgets_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


