fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core REQUIRED)

fwInclude(
    ${Qt5Core_INCLUDE_DIRS}
)
fwLink(
    ${Qt5Core_LIBRARIES}
)
