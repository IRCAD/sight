
fwLoadProperties()

find_package (Boost COMPONENTS regex date_time chrono filesystem system signals REQUIRED)
fwForwardInclude(${fwCore_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES} )

