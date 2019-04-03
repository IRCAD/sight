fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex REQUIRED)
find_package(PCL QUIET REQUIRED COMPONENTS common io)

fwInclude(
    ${PCL_INCLUDE_DIRS}
)

fwLink(
    ${PCL_COMMON_LIBRARIES}
    ${PCL_IO_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
)
