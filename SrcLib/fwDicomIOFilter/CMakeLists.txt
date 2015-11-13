fwLoadProperties()
fwUseForwardInclude(
       fwCore
       fwTools
       fwMath
       fwData
       fwMedData
       fwDicomData
       fwLog
)

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )
