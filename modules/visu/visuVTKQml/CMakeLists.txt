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

message(DEPRECATION "The VTK renderer is deprecated and will be removed in sight 22.0.
                     Please switch to the Ogre renderer.")
