sight_add_target( ui_base TYPE LIBRARY PCH pchServices )

target_link_libraries(ui_base PUBLIC OpenSSL::Crypto)

target_link_libraries(ui_base PUBLIC core data service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
