
fwLoadProperties()

# This is the only place where we should pass these variables to the C++ code
target_compile_definitions(${FWPROJECT_NAME} PRIVATE BUNDLE_LIB_PREFIX="${BUNDLE_LIB_PREFIX}")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE BUNDLE_RC_PREFIX="${BUNDLE_RC_PREFIX}")

target_link_libraries(${FWPROJECT_NAME} PUBLIC Boost::regex ${LIBXML2_LIBRARIES})

fwForwardInclude(
    ${LIBXML2_INCLUDE_DIR}
    )
