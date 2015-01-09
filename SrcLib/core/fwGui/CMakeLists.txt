fwEnableCpp11()
fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwRuntime
    fwServices
    fwTools
)


find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})


