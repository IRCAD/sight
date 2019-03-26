fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets Qml Quick Concurrent REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

set(QML_IMPORT_PATH "${QML_IMPORT_PATH};${${FWPROJECT_NAME}_RC_BUILD_DIR}" CACHE string "CMake Components" FORCE)
