
fwLoadProperties()

find_package(ZEROMQ QUIET REQUIRED)
find_package(OpenIGTLink QUIET REQUIRED)

fwForwardInclude(
    ${ZeroMQ_INCLUDE_DIRS}
    ${OpenIGTLink_INCLUDE_DIRS}
)

fwForwardLink(
    ${ZeroMQ_LIBRARY}
    ${OpenIGTLink_LIBRARIES}
)
