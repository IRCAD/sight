fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwMath
    fwMedData
    fwRuntime
    fwTools
    fwDataCamp
)

find_package (Boost COMPONENTS regex REQUIRED)
fwLink(${Boost_REGEX_LIBRARY})
