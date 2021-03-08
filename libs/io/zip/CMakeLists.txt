sight_add_target( io_zip TYPE LIBRARY )

find_package(ZLIB QUIET REQUIRED )
target_include_directories(io_zip SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(io_zip PRIVATE ${ZLIB_LIBRARIES})

target_compile_definitions(io_zip PRIVATE "-DINCLUDECRYPTINGCODE_IFCRYPTALLOWED")
target_compile_definitions(io_zip PRIVATE "-DHAVE_AES")
target_compile_definitions(io_zip PRIVATE "-DUSE_INTEL_AES_IF_PRESENT")

target_link_libraries(io_zip PUBLIC core)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
