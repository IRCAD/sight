
fwLoadProperties()

fwUseForwardInclude(fwCore fwThread)

find_package (Boost COMPONENTS thread REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS} )
fwLink(${Boost_LIBRARIES})



