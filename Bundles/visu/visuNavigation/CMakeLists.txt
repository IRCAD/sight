fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui REQUIRED)
fwForwardInclude(${QT_INCLUDE_DIR})
fwLink(${QT_LIBRARIES})

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)
