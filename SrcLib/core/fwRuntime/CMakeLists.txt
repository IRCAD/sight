
fwLoadProperties()

# This is the only place where we should pass these variables to the C++ code
target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")

target_link_libraries(${FWPROJECT_NAME} PUBLIC ${LIBXML2_LIBRARIES})

fwForwardInclude(
    ${LIBXML2_INCLUDE_DIR}
)
if(WIN32)
    fwForwardInclude(
        ${Iconv_INCLUDE_DIR}
    )
endif()
