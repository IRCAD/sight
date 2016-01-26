fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwData
    fwTools
)

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

