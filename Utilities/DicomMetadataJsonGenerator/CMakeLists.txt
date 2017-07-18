fwLoadProperties()

find_package(Boost COMPONENTS program_options REQUIRED)
fwLink(${Boost_LIBRARIES})

find_package(DCMTK REQUIRED)
fwInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )
