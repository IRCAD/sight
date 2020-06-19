
fwLoadProperties()

find_package(OpenIGTLink QUIET REQUIRED)

fwForwardInclude(
    ${OpenIGTLink_INCLUDE_DIRS}
)

fwForwardLink(
    ${OpenIGTLink_LIBRARIES}
)
