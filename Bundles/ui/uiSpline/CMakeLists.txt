fwLoadProperties()

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

