fwLoadProperties()
fwUseForwardInclude(
    fwGui
    fwServices
    fwTools
)

find_package(Boost COMPONENTS python REQUIRED)

fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})


