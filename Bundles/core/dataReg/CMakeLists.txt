fwLoadProperties()


find_package (Boost COMPONENTS thread system filesystem REQUIRED)

fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)


