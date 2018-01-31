fwLoadProperties()

find_package(Boost QUIET COMPONENTS program_options REQUIRED)
fwLink(${Boost_LIBRARIES})

find_package(DCMTK QUIET REQUIRED)
fwInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )
