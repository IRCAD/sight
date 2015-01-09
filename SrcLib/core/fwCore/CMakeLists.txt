fwLoadProperties()

find_package (Boost COMPONENTS regex filesystem date_time chrono log log_setup REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

unset(Boost_LIBRARIES)
find_package (Boost COMPONENTS thread system REQUIRED)
fwForwardLink(${Boost_LIBRARIES})

