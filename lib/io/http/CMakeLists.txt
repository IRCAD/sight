sight_add_target(io_http TYPE LIBRARY)

find_package(Qt6 QUIET COMPONENTS Network REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Network)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

find_package(CURL QUIET REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE CURL::libcurl)

target_link_libraries(${SIGHT_TARGET} PUBLIC core data ui)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
