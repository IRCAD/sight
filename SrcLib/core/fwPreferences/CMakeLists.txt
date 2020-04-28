fwLoadProperties()

find_package(OpenSSL QUIET COMPONENTS OpenSSL::Crypto)
target_link_libraries(${FWPROJECT_NAME} PUBLIC ${OPENSSL_CRYPTO_LIBRARIES})
fwForwardInclude(${OPENSSL_INCLUDE_DIR})
