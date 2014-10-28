
fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwData
    fwServices
    fwTools
)

find_package (Boost COMPONENTS signals REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})


