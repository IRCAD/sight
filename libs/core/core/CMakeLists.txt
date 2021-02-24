sight_add_target( core TYPE LIBRARY OBJECT_LIBRARY ON WARNINGS_AS_ERRORS ON )

find_package(Boost QUIET COMPONENTS date_time log log_setup iostreams REQUIRED)

target_link_libraries(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC std::filesystem)
target_link_libraries(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC Boost::date_time Boost::log Boost::log_setup Boost::iostreams)

target_include_directories(${FWPROJECT_NAME_OBJECT_LIB} SYSTEM PUBLIC ${CAMP_INCLUDE_DIR} )

target_link_libraries(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC ${CAMP_LIBRARIES})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_compile_definitions(${FWPROJECT_NAME_OBJECT_LIB} PRIVATE "-DWIN32_LEAN_AND_MEAN")
    target_link_libraries(${FWPROJECT_NAME_OBJECT_LIB} PRIVATE psapi)
    # Boost >= 1.67 changes the default random provider on Windows to use BCrypt
    target_link_libraries(${FWPROJECT_NAME_OBJECT_LIB} PRIVATE bcrypt)
endif()

# This is the only place where we should pass these variables to the C++ code
target_compile_definitions(${FWPROJECT_NAME_OBJECT_LIB} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")
target_compile_definitions(${FWPROJECT_NAME_OBJECT_LIB} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")

target_include_directories(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC ${LIBXML2_INCLUDE_DIR})
target_link_libraries(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC ${LIBXML2_LIBRARIES})

if(WIN32)
    target_include_directories(${FWPROJECT_NAME_OBJECT_LIB} PUBLIC ${Iconv_INCLUDE_DIR})
endif()

if(BUILD_TESTS)
    add_subdirectory(test/api)
    add_subdirectory(test/detail)
endif(BUILD_TESTS)
