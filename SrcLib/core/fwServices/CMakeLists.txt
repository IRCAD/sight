
fwLoadProperties()

fwUseForwardInclude(
    fwActivities
    fwCom
    fwCore
    fwData
    fwRuntime
    fwThread
    fwTools
    )

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

