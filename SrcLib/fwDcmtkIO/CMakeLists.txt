fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwDataIO
    fwTools
    fwThread
    fwMedData
    fwCom
    fwComEd
    fwDicomData
    fwZip
    fwDicomIOFilter
    fwDicomTools
    fwDcmtkTools
)

#racy compatibility

find_package(Boost COMPONENTS filesystem regex REQUIRED)
fwForwardInclude( ${Boost_INCLUDE_DIRS} )
fwLink( ${Boost_REGEX_LIBRARY} )

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )
