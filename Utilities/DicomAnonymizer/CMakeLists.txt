fwLoadProperties()


find_package(Boost COMPONENTS filesystem program_options REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

