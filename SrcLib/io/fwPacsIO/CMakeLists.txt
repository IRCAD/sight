fwLoadProperties()

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwForwardLink( ${DCMTK_LIBRARIES} )
