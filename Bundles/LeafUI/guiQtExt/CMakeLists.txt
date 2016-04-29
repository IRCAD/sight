fwLoadProperties()

find_package(Qt5 COMPONENTS Core Gui REQUIRED)
fwLink(
    ${Qt5Core_LIBRARIES} 
    ${Qt5Gui_LIBRARIES} 
)
fwInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)


