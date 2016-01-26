
fwLoadProperties()

fwUseForwardInclude(fwCore)

find_package (Boost COMPONENTS regex signals REQUIRED)
fwForwardInclude(${fwCore_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

