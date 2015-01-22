fwLoadProperties()

fwUseForwardInclude(
    fwData
    fwDataTools
    fwGui
    fwRuntime
    fwServices

    io
)

find_package (Boost COMPONENTS regex REQUIRED)
fwLink(${Boost_REGEX_LIBRARY})

