fwLoadProperties()

find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(Qt5 COMPONENTS Core Gui Network REQUIRED)
fwForwardInclude(
    ${Qt5Core_INCLUDE_DIR}
    ${Qt5Gui_INCLUDE_DIR}
    ${Qt5NetWork_INCLUDE_DIR}
)
fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Network_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
