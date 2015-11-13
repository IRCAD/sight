fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwData
    fwTools
    fwGui
    fwLog
)

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

