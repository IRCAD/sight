fwLoadProperties()

find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

fwForwardInclude(
    ${Qt5PrintSupport_INCLUDE_DIRS}
)

fwLink(
    ${Qt5PrintSupport_LIBRARIES}
)
