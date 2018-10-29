fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Qml REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

set(QML_IMPORT_PATH "${QML_IMPORT_PATH};${${FWPROJECT_NAME}_RC_BUILD_DIR}" CACHE string "CMake Components" FORCE)
