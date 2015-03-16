fwLoadProperties()
fwUseForwardInclude(
    fwCom
    fwActivities
    arAndroidTools
    fwRuntime
    fwServices
    fwTools
)

find_package(Boost COMPONENTS signals REQUIRED)

fwLink(
    ${Boost_SIGNALS_LIBRARY}
)




