
fwLoadProperties()

find_package(LibArchive REQUIRED)
find_package(OpenIGTLink REQUIRED)
find_library(OpenIGTLink_LIBRARY NAMES ${OpenIGTLink_LIBRARIES} PATHS ${OpenIGTLink_LIBRARY_DIRS})

fwForwardInclude(
    ${OpenIGTLink_INCLUDE_DIRS}
    ${LibArchive_INCLUDE_DIRS}
)
fwLink(
    ${LibArchive_LIBRARIES}
)
fwForwardLink(
    ${OpenIGTLink_LIBRARY}
)

