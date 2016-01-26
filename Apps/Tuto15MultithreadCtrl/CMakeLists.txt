fwLoadProperties()

fwUseForwardInclude(
    fwData
    fwServices
    fwCom
    fwThread
    fwComEd
    fwRuntime
    fwTools
    fwCore
)

find_package (Boost COMPONENTS log REQUIRED)

fwLink(
    ${Boost_LOG_LIBRARY}
)
