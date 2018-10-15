fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex REQUIRED)
fwLink(${Boost_REGEX_LIBRARY})
