fwLoadProperties()
fwUseForwardInclude(
    fwGui
    fwRuntime
    fwTools
)


find_package(Boost REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    )

