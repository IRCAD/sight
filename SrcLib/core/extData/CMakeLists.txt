
fwLoadProperties()
fwUseForwardInclude(
    fwCamp
    fwCom
    fwCore
    fwData
)

find_package (Boost COMPONENTS log REQUIRED)

fwForwardInclude(${Boost_INCLUDE_DIRS})

fwLink(${Boost_LOG_LIBRARY})

