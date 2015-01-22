fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwData
    fwRuntime
    fwServices
)

find_package (Boost COMPONENTS regex REQUIRED)
fwLink(${Boost_REGEX_LIBRARY})
