fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

set(QML_IMPORT_PATH "${QML_IMPORT_PATH};${${FWPROJECT_NAME}_RC_BUILD_DIR}" CACHE STRING "Path of the Qml modules" FORCE)
