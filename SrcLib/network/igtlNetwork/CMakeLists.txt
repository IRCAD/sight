
fwLoadProperties()

find_package(OpenIGTLink REQUIRED)
find_library(OpenIGTLink_LIBRARY NAMES ${OpenIGTLink_LIBRARIES} PATHS ${OpenIGTLink_LIBRARY_DIRS})

fwForwardInclude(
    ${OpenIGTLink_INCLUDE_DIRS}
)

fwForwardLink(
    ${OpenIGTLink_LIBRARY}
)
