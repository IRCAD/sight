
fwLoadProperties()

find_package (Boost COMPONENTS regex REQUIRED)
find_package (LibXml2)
find_package (Iconv REQUIRED)

fwForwardInclude(
    ${LIBXML2_INCLUDE_DIR}
    ${ICONV_INCLUDE_DIR}
    )

fwLink(
    ${LIBXML2_LIBRARIES} 
    ${Boost_REGEX_LIBRARY}
    ${ICONV_LIBRARIES}
    )

