
fwLoadProperties()

find_package (Boost COMPONENTS regex REQUIRED)
find_package (LibXml2)

fwForwardInclude(
    ${LIBXML2_INCLUDE_DIR}
    )

fwLink(
    ${LIBXML2_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
    )
