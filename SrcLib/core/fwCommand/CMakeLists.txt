fwLoadProperties()

fwUseForwardInclude(
    fwComEd
    fwData
    fwServices
    fwTools
)

find_package(Boost COMPONENTS signals REQUIRED)
fwLink(${Boost_SIGNALS_LIBRARY})


