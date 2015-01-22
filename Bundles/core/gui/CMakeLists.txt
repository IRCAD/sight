fwLoadProperties()
fwUseForwardInclude(
    fwCom
    fwCore
    fwGui
    fwRuntime
    fwServices
    fwTools
)

find_package (Boost COMPONENTS regex REQUIRED)

fwForwardInclude(${Boost_INCLUDE_DIRS})

fwLink(
    ${Boost_REGEX_LIBRARY}
)

