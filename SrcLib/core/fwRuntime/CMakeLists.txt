
fwLoadProperties()

# This is the only place where we should pass these variables to the C++ code
target_compile_definitions(${FWPROJECT_NAME} PRIVATE BUNDLE_LIB_PREFIX="${BUNDLE_LIB_PREFIX}")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE BUNDLE_RC_PREFIX="${BUNDLE_RC_PREFIX}")

find_package(Boost QUIET COMPONENTS regex REQUIRED)
find_package(LibXml2 QUIET REQUIRED)

fwForwardInclude(
    ${LIBXML2_INCLUDE_DIR}
)

fwLink(
    ${LIBXML2_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
)

if(WIN32)
    find_package(iconv QUIET REQUIRED)
    fwForwardInclude(
        ${Iconv_INCLUDE_DIR}
    )
endif()