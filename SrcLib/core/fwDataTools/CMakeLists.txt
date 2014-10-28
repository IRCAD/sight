
fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwCore
    fwData
    fwMath
    fwTools
)

find_package (Boost COMPONENTS thread REQUIRED)
fwForwardInclude(${fwCore_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

