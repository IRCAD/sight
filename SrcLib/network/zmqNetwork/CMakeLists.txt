
fwLoadProperties()

find_package(ZEROMQ REQUIRED)
find_package(OpenIGTLink REQUIRED)

fwForwardInclude(
    ${ZeroMQ_INCLUDE_DIRS}
    ${OpenIGTLink_INCLUDE_DIRS}
)

fwForwardLink(
    ${ZeroMQ_LIBRARY}
    ${OpenIGTLink_LIBRARIES}
)
