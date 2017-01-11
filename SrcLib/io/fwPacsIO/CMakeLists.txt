fwLoadProperties()


#racy compatibility

find_package(Boost COMPONENTS filesystem REQUIRED)

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwForwardLink( ${DCMTK_LIBRARIES} )
