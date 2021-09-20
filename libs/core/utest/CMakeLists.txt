sight_add_target( utest TYPE LIBRARY )

target_link_libraries(utest PUBLIC CppUnit)
target_link_libraries(utest PUBLIC core)
