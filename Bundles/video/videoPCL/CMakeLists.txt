fwLoadProperties()

fwQuickFindPCL()

find_package(Boost QUIET COMPONENTS regex REQUIRED)

fwInclude(
    ${PCL_INCLUDE_DIRS}
)

fwLink(
    ${PCL_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
)
