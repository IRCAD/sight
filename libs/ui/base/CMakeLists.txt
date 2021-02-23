fwLoadProperties()

target_link_libraries(${FWPROJECT_NAME} PUBLIC OpenSSL::Crypto)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      service
)
