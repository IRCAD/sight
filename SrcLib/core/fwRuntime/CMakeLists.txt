
fwLoadProperties()

fwUseForwardInclude(fwCore)


find_package (Boost COMPONENTS regex filesystem system date_time REQUIRED)
find_package (LibXml2)
find_package (Iconv REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS} 
    ${LIBXML2_INCLUDE_DIR}
    ${ICONV_INCLUDE_DIR}
    )

fwLink(
    ${LIBXML2_LIBRARIES} 
    ${Boost_LIBRARIES}
    ${ICONV_LIBRARIES}
    )

