
fwLoadProperties()

fwUseForwardInclude(
    fwAtoms
    fwCamp
    fwCore
    fwMemory
    fwTools
)

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})



