fwLoadProperties()

find_package (Boost COMPONENTS regex signals REQUIRED)
fwLink(${Boost_LIBRARIES})
