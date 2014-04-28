
fwLoadProperties()
fwUseForwardInclude(
    fwCom
    fwData
    fwMedData
    fwMemory
    fwRuntime
    fwServices
    fwTools
)

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})






