sight_add_target( data TYPE LIBRARY WARNINGS_AS_ERRORS ON )

find_package(CAMP QUIET REQUIRED)
target_include_directories(${FWPROJECT_NAME} SYSTEM PRIVATE ${CAMP_INCLUDE_DIR})
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${CAMP_LIBRARIES})

target_link_libraries(${FWPROJECT_NAME} PUBLIC core )
