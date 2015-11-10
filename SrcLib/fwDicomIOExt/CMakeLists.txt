fwLoadProperties()

fwUseForwardInclude(
       fwCore
       fwData
       fwDataIO
       fwTools
       fwThread
       fwMedData
       fwCom
       fwComEd
       fwDicomData
       fwDicomIOFilter
       fwMemory
       fwRuntime
       fwZip
       vtkGdcmIO
       fwVtkIO
)

find_package(Boost COMPONENTS regex REQUIRED)
find_package(DCMTK REQUIRED)
find_package(Iconv REQUIRED)
find_package(ZLIB REQUIRED)
find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

fwInclude(
    ${ZLIB_INCLUDE_DIRS}
    ${ICONV_INCLUDE_DIR}
)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS}
    ${DCMTK_config_INCLUDE_DIRS}
    ${GDCM_INCLUDE_DIRS}
)

fwLink(
    ${Boost_REGEX_LIBRARIES}
    ${ICONV_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

fwForwardLink(
    ${DCMTK_LIBRARIES}
    ${GDCM_LIBRARIES}

    gdcmCommon
    gdcmMSFF
)
