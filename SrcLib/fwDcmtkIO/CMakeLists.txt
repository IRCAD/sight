fwLoadProperties()


#racy compatibility

find_package(Boost COMPONENTS filesystem regex REQUIRED)
fwForwardInclude( ${Boost_INCLUDE_DIRS} )
fwLink( ${Boost_REGEX_LIBRARY} )

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )
