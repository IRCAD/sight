sight_add_target( io_zip TYPE LIBRARY )

find_package(ZLIB QUIET REQUIRED )
target_include_directories(${FWPROJECT_NAME} SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${ZLIB_LIBRARIES})

target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DINCLUDECRYPTINGCODE_IFCRYPTALLOWED")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DHAVE_AES")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DUSE_INTEL_AES_IF_PRESENT")

target_link_libraries(${FWPROJECT_NAME} PUBLIC core)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
