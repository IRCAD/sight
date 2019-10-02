fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui Quick Qml QuickControls2 REQUIRED)


fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
    ${Qt5QuickControls2_INCLUDE_DIRS}
)

fwLink(
       ${Qt5Core_LIBRARIES}
       ${Qt5Gui_LIBRARIES}
       ${Qt5Quick_LIBRARIES}
       ${Qt5Qml_LIBRARIES}
       ${Qt5QuickControls2_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
set(QML_IMPORT_PATH "${QML_IMPORT_PATH};${${FWPROJECT_NAME}_RC_BUILD_DIR}" CACHE STRING "Path of the Qml modules" FORCE)
