
fwLoadProperties()

find_package(OpenIGTLink REQUIRED)

fwForwardInclude(
    ${OpenIGTLink_INCLUDE_DIRS}
)

fwForwardLink(
    ${OpenIGTLink_LIBRARIES}
)
