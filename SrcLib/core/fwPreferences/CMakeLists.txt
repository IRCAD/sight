fwLoadProperties()

find_package(OpenSSL QUIET)
target_link_libraries(${FWPROJECT_NAME} PUBLIC OpenSSL::Crypto)
