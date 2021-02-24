sight_add_target( utest TYPE LIBRARY WARNINGS_AS_ERRORS ON )

target_link_libraries(${FWPROJECT_NAME} PUBLIC CppUnit)
target_link_libraries(${FWPROJECT_NAME} PUBLIC core)
