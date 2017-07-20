fwLoadProperties()

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )
