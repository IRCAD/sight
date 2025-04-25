sight_add_target(io_http_ut TYPE TEST)

find_package(Qt6 QUIET COMPONENTS Network REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE Qt6::Network)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PRIVATE "QT_NO_KEYWORDS")

target_link_libraries(${SIGHT_TARGET} PRIVATE io_http core ui_qt utest_data data)
