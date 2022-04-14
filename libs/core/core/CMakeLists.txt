sight_add_target(core TYPE LIBRARY OBJECT_LIBRARY ON)

find_package(Boost QUIET COMPONENTS date_time log log_setup iostreams REQUIRED)

target_link_libraries(${TARGET_OBJECT_LIB} PUBLIC std::filesystem)
target_link_libraries(${TARGET_OBJECT_LIB} PUBLIC Boost::date_time Boost::log Boost::log_setup Boost::iostreams)

target_compile_definitions(${TARGET_OBJECT_LIB} PUBLIC BOOST_BIND_GLOBAL_PLACEHOLDERS)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE psapi)
    # Boost >= 1.67 changes the default random provider on Windows to use BCrypt
    target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE bcrypt)
endif()

# This is the only place where we should pass these variables to the C++ code
target_compile_definitions(${TARGET_OBJECT_LIB} PRIVATE MODULE_BIN_PREFIX="${CMAKE_INSTALL_BINDIR}")
target_compile_definitions(${TARGET_OBJECT_LIB} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")
target_compile_definitions(${TARGET_OBJECT_LIB} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")

target_include_directories(${TARGET_OBJECT_LIB} PUBLIC ${LIBXML2_INCLUDE_DIR})
target_link_libraries(${TARGET_OBJECT_LIB} PUBLIC ${LIBXML2_LIBRARIES})

# Include openssl for crypto
target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE OpenSSL::Crypto)

if(UNIX)
    # We use dlopen, dlerror, ...
    target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE ${CMAKE_DL_LIBS})
endif()

if(WIN32)
    target_include_directories(${TARGET_OBJECT_LIB} PUBLIC ${Iconv_INCLUDE_DIR})
endif()

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/api)
    add_subdirectory(test/detail)
endif(SIGHT_BUILD_TESTS)
