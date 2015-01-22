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

find_package(Boost COMPONENTS regex REQUIRED)
fwLink(${Boost_REGEX_LIBRARY})
fwForwardInclude(${Boost_INCLUDE_DIRS})
