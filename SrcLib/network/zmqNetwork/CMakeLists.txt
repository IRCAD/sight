
fwLoadProperties()

find_package(ZEROMQ REQUIRED)
find_package(OpenIGTLink REQUIRED)
find_library(OpenIGTLink_LIBRARY NAMES ${OpenIGTLink_LIBRARIES} PATHS ${OpenIGTLink_LIBRARY_DIRS})

fwForwardInclude(
    ${ZeroMQ_INCLUDE_DIRS}
    ${OpenIGTLink_INCLUDE_DIRS}
)

fwForwardLink(
    ${ZeroMQ_LIBRARY}
    ${OpenIGTLink_LIBRARY}
)
