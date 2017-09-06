fwLoadProperties()

fwQuickFindPCL()

find_package(Boost COMPONENTS regex REQUIRED)

fwInclude(
    ${PCL_INCLUDE_DIRS}
)

fwLink(
    ${PCL_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
)
