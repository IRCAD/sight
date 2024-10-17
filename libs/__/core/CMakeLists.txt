sight_add_target(core TYPE LIBRARY OBJECT_LIBRARY ON)

target_link_libraries(${TARGET_OBJECT_LIB} PUBLIC std::filesystem)
target_link_libraries(${TARGET_OBJECT_LIB} PUBLIC Boost::date_time Boost::log Boost::log_setup Boost::iostreams)

target_compile_definitions(${TARGET_OBJECT_LIB} PUBLIC BOOST_BIND_GLOBAL_PLACEHOLDERS)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE psapi)

    # Boost >= 1.67 changes the default random provider on Windows to use BCrypt
    target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE bcrypt)

    # For timeBeginPeriod  and timeEndPeriod
    target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE winmm)
endif()

# This is the only place where we should pass these variables to the C++ code
target_compile_definitions(${TARGET_OBJECT_LIB} PRIVATE MODULE_BIN_PREFIX="${CMAKE_INSTALL_BINDIR}")
target_compile_definitions(${TARGET_OBJECT_LIB} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")
target_compile_definitions(${TARGET_OBJECT_LIB} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")

target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE LibXml2::LibXml2)

# Include openssl for crypto
target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE OpenSSL::Crypto)

if(UNIX)
    # We use dlopen, dlerror, ...
    target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE ${CMAKE_DL_LIBS})
endif()

target_link_libraries(${TARGET_OBJECT_LIB} PUBLIC OpenMP::OpenMP_CXX)

if(NOT WIN32 AND ZSTD_FOUND)
    target_include_directories(${TARGET_OBJECT_LIB} SYSTEM PRIVATE ${ZSTD_INCLUDE_DIRS})
    target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE ${ZSTD_LINK_LIBRARIES})
else()
    target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE zstd::libzstd_shared)
endif()

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/api)
    add_subdirectory(test/detail)
endif(SIGHT_BUILD_TESTS)
