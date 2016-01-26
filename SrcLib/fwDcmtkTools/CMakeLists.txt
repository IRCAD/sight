fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwTools
    fwComEd
)

find_package(Boost COMPONENTS filesystem REQUIRED)

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )
