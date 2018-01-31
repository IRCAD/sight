fwLoadProperties()

find_package(Boost QUIET COMPONENTS program_options REQUIRED)
fwLink(${Boost_LIBRARIES})
