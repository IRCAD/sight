fwLoadProperties()

find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
    )
fwForwardInclude(${QT_INCLUDE_DIR})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

