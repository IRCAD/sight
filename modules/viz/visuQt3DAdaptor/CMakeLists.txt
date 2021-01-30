fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS
    Core
 REQUIRED)

fwInclude(
    ${Qt5Core_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)