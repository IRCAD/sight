fwLoadProperties()

find_package (Boost COMPONENTS regex REQUIRED)

fwUseForwardInclude(
    fwCom
    fwCore
    fwData
    fwRuntime
    fwTools
    fwServices
)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS}
)

fwForwardLink(
    ${Boost_REGEX_LIBRARY}
)
