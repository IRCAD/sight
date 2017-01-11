fwLoadProperties()

find_package(Boost COMPONENTS system REQUIRED)

fwInclude(
    ${Boost_INCLUDE_DIRS}
)

fwLink(
    ${Boost_SYSTEM_LIBRARY}
)
