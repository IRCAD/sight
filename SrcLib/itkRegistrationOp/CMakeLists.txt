fwLoadProperties()

find_package(ITK QUIET REQUIRED)

fwForwardLink(
    ITKOptimizersv4
    ITKTransform
)
