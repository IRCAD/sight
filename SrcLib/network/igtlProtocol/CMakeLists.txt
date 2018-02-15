
fwLoadProperties()

find_package(LibArchive REQUIRED)
find_package(OpenIGTLink REQUIRED)

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

