
fwLoadProperties()

# This is the only place where we should pass these variables to the C++ code
target_compile_definitions(${FWPROJECT_NAME} PRIVATE BUNDLE_LIB_PREFIX="${BUNDLE_LIB_PREFIX}")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE BUNDLE_RC_PREFIX="${BUNDLE_RC_PREFIX}")

find_package (Boost COMPONENTS regex REQUIRED)
find_package (LibXml2)

fwForwardInclude(
    ${LIBXML2_INCLUDE_DIR}
    )

fwLink(
    ${LIBXML2_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
    )
