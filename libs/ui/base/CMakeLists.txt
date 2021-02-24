sight_add_target( ui_base TYPE LIBRARY PCH pchServices )

target_link_libraries(${FWPROJECT_NAME} PUBLIC OpenSSL::Crypto)

target_link_libraries(${FWPROJECT_NAME} PUBLIC core data service)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
