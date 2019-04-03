fwLoadProperties()

find_package(ITK QUIET COMPONENTS ITKOptimizersv4 ITKTransform REQUIRED)

fwForwardLink(
    ITKOptimizersv4
    ITKTransform
)
