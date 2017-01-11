fwLoadProperties()

find_package(Boost COMPONENTS date_time REQUIRED)

fwForwardInclude(${Boost_INCLUDE_DIRS})

fwLink(${Boost_DATE_TIME_LIBRARY} igtlNetwork)
