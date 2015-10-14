fwLoadProperties()

fwUseForwardInclude(
    fwComEd
    fwCore
    fwData
    fwMath
    fwTools
)

find_package(GLM REQUIRED)

fwForwardInclude(
    ${GLM_INCLUDE_DIR}
)
