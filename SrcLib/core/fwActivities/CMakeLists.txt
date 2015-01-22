fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwMedData
    fwRuntime
    fwTools
)

find_package (Boost COMPONENTS regex REQUIRED)
fwLink(${Boost_REGEX_LIBRARY})
