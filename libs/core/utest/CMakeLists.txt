sight_add_target( utest TYPE LIBRARY WARNINGS_AS_ERRORS ON )

target_link_libraries(utest PUBLIC CppUnit)
target_link_libraries(utest PUBLIC core)
