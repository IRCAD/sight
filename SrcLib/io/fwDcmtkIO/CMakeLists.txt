fwLoadProperties()

find_package(Boost COMPONENTS regex REQUIRED)
fwLink( ${Boost_REGEX_LIBRARY} )

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )
