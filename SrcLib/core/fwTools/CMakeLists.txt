fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex signals REQUIRED)
fwLink(${Boost_LIBRARIES})
