sight_add_target( io_base TYPE LIBRARY PCH pchData WARNINGS_AS_ERRORS ON )

#racy compatibility
target_compile_definitions(${FWPROJECT_NAME} PRIVATE PRJ_NAME="${NAME}")

find_package(ZLIB QUIET REQUIRED )
target_include_directories(${FWPROJECT_NAME} SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(${FWPROJECT_NAME} PRIVATE  ${ZLIB_LIBRARIES})

target_link_libraries(${FWPROJECT_NAME} PUBLIC core data service)
