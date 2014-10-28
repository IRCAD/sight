fwLoadProperties()

find_package (Boost COMPONENTS filesystem program_options REQUIRED)
fwInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

