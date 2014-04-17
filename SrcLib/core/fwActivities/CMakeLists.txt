
fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwMedData
    fwRuntime
    fwTools
    )

find_package (Boost COMPONENTS date_time REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})


