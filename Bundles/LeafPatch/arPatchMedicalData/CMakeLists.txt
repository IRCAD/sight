fwLoadProperties()

find_package(Boost COMPONENTS system REQUIRED)

fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(
    ${Boost_SYSTEM_LIBRARY}
)
