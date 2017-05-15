fwLoadProperties()


find_package(Boost COMPONENTS filesystem system REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(
    ${Boost_FILESYSTEM_LIBRARY}
    ${BOOST_SYSTEM_LIBRARY}
)


