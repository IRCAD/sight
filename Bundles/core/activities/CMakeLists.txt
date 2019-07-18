fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
)

if(NOT ${QML_APPLICATION})
    find_package(Qt5 QUIET COMPONENTS Widgets REQUIRED)
    fwForwardInclude(
        ${Qt5Widgets_INCLUDE_DIRS}
    )
    fwLink(
        ${Qt5Widgets_LIBRARIES}
    )
endif()

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
