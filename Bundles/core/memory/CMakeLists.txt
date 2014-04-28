fwLoadProperties()
fwUseForwardInclude(
    fwMemory
    fwRuntime
    fwServices
    fwTools
)

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})


