
fwLoadProperties()

find_package(LibArchive QUIET REQUIRED)
find_package(OpenIGTLink QUIET REQUIRED)

fwForwardInclude(
    ${OpenIGTLink_INCLUDE_DIRS}
    ${LibArchive_INCLUDE_DIRS}
)
fwLink(
    ${LibArchive_LIBRARIES}
)
fwForwardLink(
    ${OpenIGTLink_LIBRARIES}
)

